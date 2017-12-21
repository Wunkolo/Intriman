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

class RoffGenerator : public Intriman::IGenerator
{
public:
	explicit RoffGenerator(const std::experimental::filesystem::path& Folder);

	virtual ~RoffGenerator() override;

	virtual void StartList(const char* Version, const char* Date) override;

	virtual void VisitIntrinsic(const Intriman::Intrinsic& CurIntrin) override;

	virtual void EndList() override;

private:
	std::string Version;
	std::string Date;
	fs::path CurPath;
	std::unordered_map<std::string, fs::path> SubFolders;
};


RoffGenerator::RoffGenerator(const fs::path& Folder)
{
	CurPath = Folder / "Roff";
	fs::create_directories(CurPath);
}

RoffGenerator::~RoffGenerator()
{
}

void RoffGenerator::StartList(const char* Version, const char* Date)
{
	this->Version = Version;
	this->Date = Date;
}

void RoffGenerator::VisitIntrinsic(const Intriman::Intrinsic& CurIntrin)
{
	// New technology detected
	if( !SubFolders.count(CurIntrin.Technology) )
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

		const fs::path Path = (CurPath / TechName);
		fs::create_directories(Path);
		// New subfolder for technology
		SubFolders[CurIntrin.Technology] = Path;
	}

	// New file for instruction
	const fs::path FilePath = SubFolders[CurIntrin.Technology] / CurIntrin.Name;
	std::ofstream Stream(
		FilePath
	);

	std::size_t i = 0;

	// Header
	Stream
		<< ".TH "
		<< CurIntrin.Name
		<< " \"3P\" "
		<< Version
		<< " \""
		<< CurIntrin.Technology
		<< " Intrinsics"
		<< "\" "
		<< "\n";

	// Intrinsic 
	if( CurIntrin.Header != nullptr )
	{
		Stream
			<< ".SH NAME\n"
			<< ".PP\n"
			<< "#include <"
			<< CurIntrin.Header
			<< ">\n";
	}

	Stream
		<< ".SH SYNOPSIS\n"
		<< ".PP\n"
		<< "\\ffI" << CurIntrin.ReturnType << "\\fP "
		<< CurIntrin.Name << '('
		<< "\n";

	i = 0;
	for( const auto& CurParam : CurIntrin.Parameters )
	{
		Stream
			<< '\t'
			<< CurParam.Type
			<< ' '
			<< "\fI" << CurParam.Name << "\fP"
			<< (i == CurIntrin.Parameters.size() - 1 ? ' ' : ',')
			<< '\n';
		++i;
	}
	Stream
		<< ");\n\n";

	// Description
	if( CurIntrin.Description != nullptr )
	{
		Stream
			<< CurIntrin.Description
			<< '\n';
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

void RoffGenerator::EndList()
{
}


std::unique_ptr<Intriman::IGenerator> Intriman::GeneratorFactory::CreateRoffGenerator(
	const Settings& Settings
)
{
	return std::make_unique<RoffGenerator>(Settings.OutputFolder);
}
