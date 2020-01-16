#include <cstdint>
#include <cstddef>
#include <cstdio>
#include <cctype>

#include <algorithm>
#include <regex>
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
	CurPath = Folder / "roff";
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
		<< " 3 "
		<< Date
		<< " \""
		<< CurIntrin.Technology
		<< " Intrinsics\" \n";

	Stream
		<< ".SH NAME\n"
		<< CurIntrin.Name
		<< '\n';

	// Syntax
	Stream
		<< ".SH SYNTAX\n"
		<< "\\f[B]#include <\\f[]"
		<< "\\f[I]"
		<< CurIntrin.Header
		<< "\\f[]\\f[B]>\\f[]\n.sp\n.sp\n";

	Stream
		<< CurIntrin.ReturnType
		<< " \\f[B]" << CurIntrin.Name << "\\f[]( ";

	i = 0;
	if( CurIntrin.Parameters.size() > 3 )
	{
		Stream << "\n.br\n";
		for( const auto& CurParam : CurIntrin.Parameters )
		{
			Stream << '\t' <<  CurParam.Type;

			if( CurParam.Name != nullptr )
			{
				Stream
					<< "\t\\f[I]" << CurParam.Name << "\\f[]";
			}
			Stream
				<< (i == CurIntrin.Parameters.size() - 1 ? "\n.br\n" : ",\n.br\n");
			++i;
		}
	}
	else
	{
		for( const auto& CurParam : CurIntrin.Parameters )
		{
			Stream
				<< CurParam.Type;

			if( CurParam.Name != nullptr )
			{
				Stream
					<< " \\f[I]" << CurParam.Name << "\\f[]";
			}
			Stream
				<< (i == CurIntrin.Parameters.size() - 1 ? "" : ", ");
			++i;
		}
	}
	Stream << ");\n";

	// Description
	if( CurIntrin.Description != nullptr )
	{
		Stream
			<< ".SH DESCRIPTION\n"
			<< CurIntrin.Description
			<< '\n';
	}

	// Operation
	if( CurIntrin.Operation != nullptr )
	{
		std::string Pseudo(CurIntrin.Operation);
		Pseudo = std::regex_replace(
			Pseudo,
			std::regex("\n"),
			"\n.br\n"
		);
		Stream
			<< ".SH OPERATION\n"
			<< Pseudo
			<< '\n';
	}

	// CPUID
	if( CurIntrin.CPUID.size() )
	{
		Stream
			<< ".SH CPUID\n";
		i = 0;
		for( const auto& CurCPUID : CurIntrin.CPUID )
		{
			Stream
				<< "\\f[I]"
				<< CurCPUID
				<< "\\f[]"
				<< (i == CurIntrin.CPUID.size() - 1 ? ' ' : ',');
			++i;
		}
		Stream << '\n';
	}

	// Instructions
	if( CurIntrin.Instructions.size() )
	{
		Stream
			<< ".SH ASSEMBLY\n"
			<< ".TS\nALLBOX;\nl c.\n"
			<< "\\f[B]Instruction\tOperands\\f[]\n";
		for( const auto& CurInstruction : CurIntrin.Instructions )
		{
			Stream
				<< "\\f[I]"
				<< CurInstruction.Name
				<< '\t'
				<< CurInstruction.Operands
				<< "\\f[]\n";
		}
		Stream << ".TE\n";
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
