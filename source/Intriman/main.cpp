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
		"Generates alternative documentation formats from the intel intrinsics "
		"database."
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
	PrintHelp();
	std::ifstream IntrinsicFile("intrinsics.xml");
	if( !IntrinsicFile )
	{
		std::fputs("Error opening intrinsics file: intrinsics.xml", stderr);
		return EXIT_FAILURE;
	}
	std::ifstream LatencyFile("latency.xml");
	if( !LatencyFile )
	{
		std::fputs("Error opening latency file: latency.xml", stderr);
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

	return Intriman::ProcessFile(
		IntrinsicFile, LatencyFile,
		Generators
	) ? EXIT_SUCCESS : EXIT_FAILURE;
}
