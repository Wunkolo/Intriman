#pragma once
#include <experimental/filesystem>

namespace Intriman
{
struct Settings
{
	Settings();
	std::experimental::filesystem::path OutputFolder;
};
}
