#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cctype>

#include <algorithm>
#include <fstream>
#include <iomanip>
#include <unordered_map>
#include <sstream>

#include <experimental/filesystem>
#include <Intriman/Generators/GeneratorFactory.hpp>

namespace fs = std::experimental::filesystem;

std::vector<std::string> Wrap(const std::string& String, std::size_t MaxLength)
{
	std::stringstream Stream;
	Stream << String;

	std::vector<std::string> Lines;
	while( Stream.good() ) // Copy all lines into string
	{
		std::string CurLine;
		std::getline(Stream, CurLine);
		Lines.push_back(CurLine);
	}

	std::vector<std::string> SplitString;

	for( std::size_t i = 0; i < Lines.size(); ++i )
	{
		std::string CurLine(Lines[i]);
		while( CurLine.length() > MaxLength )
		{
			std::size_t EndIndex = MaxLength;
			while( EndIndex > 0 && !std::isspace(CurLine[EndIndex]) )
			{
				--EndIndex;
			}
			if( EndIndex <= 0 )
			{
				EndIndex = MaxLength;
			}

			SplitString.push_back(CurLine.substr(0, EndIndex));
			CurLine = CurLine.substr(EndIndex + 1, std::string::npos);
		}

		SplitString.push_back(CurLine);
	}

	return SplitString;
}

class TextGenerator : public Intriman::IGenerator
{
public:
	explicit TextGenerator(const std::experimental::filesystem::path& Folder);

	virtual ~TextGenerator() override;

	virtual void StartList(const char* Version, const char* Date) override;

	virtual void VisitIntrinsic(const Intriman::Intrinsic& CurIntrin) override;

	virtual void EndList() override;

private:

	fs::path CurPath;
	std::unordered_map<std::string, std::ofstream> Subfiles;
	std::ofstream Index;
};


TextGenerator::TextGenerator(const fs::path& Folder)
{
	CurPath = Folder / "text";
	fs::create_directories(CurPath);
	Index.open(
		(CurPath / "Index.txt")
	);

	Index
		<< "Intriman\n===\n\n";
}

TextGenerator::~TextGenerator()
{
	for( auto& SubFile : Subfiles )
	{
		SubFile.second.close();
	}
	Index.close();
}

void TextGenerator::StartList(const char* Version, const char* Date)
{
	Index
		<< "```\n"
		<< "Version: " << Version << '\n'
		<< "Date: " << Date << '\n'
		<< "```\n---\n";
}

void TextGenerator::VisitIntrinsic(const Intriman::Intrinsic& CurIntrin)
{
	// New technology detected
	if( !Subfiles.count(CurIntrin.Technology) )
	{
		std::string TechName(CurIntrin.Technology);
		// All non-alphanum characters are converted to '-'
		std::replace_if(
			TechName.begin(),
			TechName.end(),
			[](char c)
		{
			return !std::isalnum(c);
		},
			'-'
		);

		const fs::path Path = (CurPath / TechName).replace_extension(".txt");
		// New file for technology
		Subfiles[CurIntrin.Technology] = std::ofstream(
			Path
		);

		// Header for new file
		Subfiles[CurIntrin.Technology]
			<< CurIntrin.Technology
			<< "\n\n";

		Index
			<< CurIntrin.Technology << '-'
			<< '\n';
	}
	std::ostream& Stream = Subfiles[CurIntrin.Technology];

	std::size_t i = 0;

	// Header
	Stream
		<< "\n\n"
		<< std::string(80, '-')
		<< "\n\n"
		<< "- " << CurIntrin.Name
		<< "\n\n";

	// Intrinsic 
	if( CurIntrin.Header != nullptr )
	{
		Stream
			<< "#include <"
			<< CurIntrin.Header
			<< ">\n\n";
	}

	Stream << CurIntrin.ReturnType << ' '
		<< CurIntrin.Name << '('
		<< "\n";
	i = 0;
	for( const auto& CurParam : CurIntrin.Parameters )
	{
		Stream
			<< '\t'
			<< CurParam.Type
			<< ' '
			<< CurParam.Name
			<< (i == CurIntrin.Parameters.size() - 1 ? ' ' : ',')
			<< '\n';
		++i;
	}
	Stream
		<< ");\n\n";

	// Description
	if( CurIntrin.Description != nullptr )
	{
		const std::vector<std::string> SplitDescription = Wrap(
			CurIntrin.Description,
			80
		);

		for( const auto& CurLine : SplitDescription )
		{
			Stream
				<< CurLine
				<< '\n';
		}
	}

	// Operation
	if( CurIntrin.Operation != nullptr )
	{
		Stream
			<< CurIntrin.Operation
			<< '\n';
	}

	// CPUID
	if( CurIntrin.CPUID.size() )
	{
		Stream
			<< "CPUID: ";
		i = 0;
		for( const auto& CurCPUID : CurIntrin.CPUID )
		{
			Stream
				<< CurCPUID
				<< (i == CurIntrin.CPUID.size() - 1 ? ' ' : ',');
			++i;
		}
		Stream
			<< "\n\n";
	}

	// Instructions
	if( CurIntrin.Instructions.size() )
	{
		Stream
			<< std::setw(16) << std::left << "Instruction"
			<< std::setw(16) << "Operands"
			<< '\n'
			<< std::string(32, '-')
			<< '\n';
		for( const auto& CurInstruction : CurIntrin.Instructions )
		{
			Stream
				<< std::setw(16) << std::left << CurInstruction.Name
				<< std::setw(16) << CurInstruction.Form
				<< '\n';
		}
	}
}

void TextGenerator::EndList()
{
}


std::unique_ptr<Intriman::IGenerator> Intriman::GeneratorFactory::CreateTextGenerator(
	const Settings& Settings
)
{
	return std::make_unique<TextGenerator>(Settings.OutputFolder);
}
