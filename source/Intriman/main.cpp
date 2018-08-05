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
	std::puts(
		"Generates alternative documentation formats from the intel intrinsics"
		"database.\n"
		"Usage: ./Intriman [data-latest.xml]"
		);
	std::puts("\t - Wunkolo <wunkolo@gmail.com>");
	std::puts("Available Generators:");
#undef GEN_ENTRY
#define GEN_ENTRY(GEN_NAME) std::puts("\t - " #GEN_NAME);
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

#undef GEN_ENTRY
#define GEN_ENTRY(GEN_NAME) \
	Generators.push_back( \
		Intriman::GeneratorFactory::Create##GEN_NAME##Generator(CurSettings)\
	);
#include <Intriman/Generators/GeneratorList.hpp>
#undef GEN_ENTRY

	return Intriman::ProcessFile(InputFile, Generators) ? EXIT_SUCCESS : EXIT_FAILURE;
}
