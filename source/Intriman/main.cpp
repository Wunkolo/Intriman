#include <cstdint>
#include <cstddef>
#include <cstdio>

#include <fstream>

#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

#include <Intriman/Intriman.hpp>

void PrintHelp()
{
	std::puts("Intriman | Build Date: " __TIMESTAMP__);
	std::puts("Available Generators:");
#undef GEN_ENTRY
#define GEN_ENTRY(GEN_NAME) std::puts("\t-" #GEN_NAME);

#include <Intriman/Generators/GeneratorList.hpp>
#undef GEN_ENTRY
}

int main(int argc, const char* argv[])
{
	if( argc < 2 )
	{
		PrintHelp();
		return EXIT_FAILURE;
	}

	std::ifstream InputFile(argv[1]);

	if( !InputFile )
	{
		std::printf(
			"Error opening file: %s",
			argv[1]
		);
		return EXIT_FAILURE;
	}

	const Intriman::Settings CurSettings;

	std::vector<std::unique_ptr<Intriman::IGenerator>> Generators;

	Generators.push_back(
		Intriman::GeneratorFactory::CreateMarkdownGenerator(CurSettings)
	);
	Generators.push_back(
		Intriman::GeneratorFactory::CreateRoffGenerator(CurSettings)
	);
	Generators.push_back(
		Intriman::GeneratorFactory::CreateTextGenerator(CurSettings)
	);

	return Intriman::ProcessFile(InputFile, Generators) ? EXIT_SUCCESS : EXIT_FAILURE;
}
