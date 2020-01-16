#pragma once

#include <istream>
#include <memory>
#include <vector>

#include "Intrinsic.hpp"
#include "Generators/GeneratorFactory.hpp"

namespace Intriman
{
bool ProcessFile(
	std::istream& IntrinsicsFile, // Data from Intel
	std::istream& LatencyFile,    // Data from uops.info
	const std::vector<std::unique_ptr<IGenerator>>& Visitor
);
}