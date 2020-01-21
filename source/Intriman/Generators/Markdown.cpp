#include <cstdint>
#include <cstddef>
#include <cstdio>

#include <fstream>
#include <algorithm>
#include <unordered_map>
#include <experimental/filesystem>

#include <Intriman/Generators/GeneratorFactory.hpp>

#include <fstream>
#include <cctype>

namespace fs = std::experimental::filesystem;

class MarkdownGenerator : public Intriman::IGenerator
{
public:
	explicit MarkdownGenerator(const std::experimental::filesystem::path& Folder);

	virtual ~MarkdownGenerator() override;

	virtual void StartList(const char* Version, const char* Date) override;

	virtual void VisitIntrinsic(const Intriman::Intrinsic& CurIntrin) override;

	virtual void EndList() override;

private:

	fs::path CurPath;
	std::unordered_map<std::string, std::ofstream> Subfiles;
	std::ofstream Index;
};


MarkdownGenerator::MarkdownGenerator(const fs::path& Folder)
{
	CurPath = Folder / "markdown";
	fs::create_directories(CurPath);
	Index.open(
		(CurPath / "Index.md")
	);

	Index
		<< "Intriman\n===\n\n";
}

MarkdownGenerator::~MarkdownGenerator()
{
	for( auto& SubFile : Subfiles )
	{
		SubFile.second.close();
	}
	Index.close();
}

void MarkdownGenerator::StartList(const char* Version, const char* Date)
{
	Index
		<< "```\n"
		<< "Version: " << Version << '\n'
		<< "Date: " << Date << '\n'
		<< "```\n---\n";
}

void MarkdownGenerator::VisitIntrinsic(const Intriman::Intrinsic& CurIntrin)
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

		const fs::path Path = (CurPath / TechName).replace_extension(".md");
		// New file for technology
		Subfiles[CurIntrin.Technology] = std::ofstream(
			Path
		);

		// Header for new file
		Subfiles[CurIntrin.Technology]
			<< CurIntrin.Technology
			<< "\n===\n\n";


		Index
			<< "### "
			<< '[' << CurIntrin.Technology << "](" << "./" << TechName << ".md)"
			<< "\n";
	}
	std::ostream& Stream = Subfiles[CurIntrin.Technology];

	std::size_t i = 0;

	// Header
	Stream
		<< "## " << CurIntrin.Name
		<< "\n\n";

	// Intrinsic 
	Stream
		<< "```cpp\n";

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
			<< ' '
			<< CurParam.Type
			<< ' '
			<< CurParam.Name
			<< (i == CurIntrin.Parameters.size() - 1 ? ' ' : ',')
			<< '\n';
		++i;
	}
	Stream
		<< ");\n```\n\n";

	// Description
	if( CurIntrin.Description != nullptr )
	{
		Stream
			//<< "Description:\n"
			<< '>'
			<< CurIntrin.Description
			<< "\n\n";
	}

	// Operation
	if( CurIntrin.Operation != nullptr )
	{
		Stream
			<< "```"
			<< CurIntrin.Operation
			<< "\n```\n\n";
	}

	// CPUID
	if( CurIntrin.CPUID.size() )
	{
		Stream
			<< "CPUID:";
		i = 0;
		for( const auto& CurCPUID : CurIntrin.CPUID )
		{
			Stream
				<< " `" << CurCPUID << '`'
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
			<< "Instruction|Arguments\n"
			<< "-|-\n";
		for( const auto& CurInstruction : CurIntrin.Instructions )
		{
			Stream
				<< CurInstruction.Name
				<< '|'
				<< CurInstruction.Operands
				<< '\n';
		}
	}

	Stream
		<< "\n---\n";
}

void MarkdownGenerator::EndList()
{
}


std::unique_ptr<Intriman::IGenerator> Intriman::GeneratorFactory::CreateMarkdownGenerator(
	const Settings& Settings
)
{
	return std::make_unique<MarkdownGenerator>(Settings.OutputFolder);
}
