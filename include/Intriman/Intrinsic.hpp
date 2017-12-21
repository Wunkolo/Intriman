#pragma once
#include <vector>
#include <ostream>
#include <istream>

namespace Intriman
{
struct Intrinsic
{
	const char* Name;
	const char* ReturnType;
	const char* Technology;

	const char* Type;
	const char* Category;
	std::vector<const char*> CPUID;

	struct Parameter
	{
		const char* Name;
		const char* Type;
	};

	std::vector<Parameter> Parameters;

	const char* Description;
	const char* Operation;

	const char* Header;

	struct Instruction
	{
		const char* Name;
		const char* Form;
	};

	std::vector<Instruction> Instructions;
};
}