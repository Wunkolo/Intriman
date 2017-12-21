#pragma once

#include <memory>

#include "IGenerator.hpp"
#include "../Settings.hpp"

namespace Intriman
{
namespace GeneratorFactory
{
/// Creates a "Create(GeneratorName)Generator(); for every Generator
#undef GEN_ENTRY
#define GEN_ENTRY(GEN_NAME) \
extern std::unique_ptr<IGenerator> Create##GEN_NAME##Generator(const Settings& Settings);

#include "GeneratorList.hpp"
#undef GEN_ENTRY

/// Creates an "e(GeneratorName)" enum for every Generator
enum class GeneratorID : std::size_t
{
#undef GEN_ENTRY
#define GEN_ENTRY(GEN_NAME) e##GEN_NAME,

#include "GeneratorList.hpp"
#undef GEN_ENTRY
};

/// Creates a "(GeneratorName)" string for every Generator
static const char* GeneratorNames[] =
{
#undef GEN_ENTRY
#define GEN_ENTRY(GEN_NAME) #GEN_NAME,

#include "GeneratorList.hpp"
#undef GEN_ENTRY
};
}
}
