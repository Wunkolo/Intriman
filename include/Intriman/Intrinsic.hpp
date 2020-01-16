#pragma once
#include <vector>
#include <ostream>
#include <istream>
#include <cmath>

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


	struct Latency
	{
		// IACA or measurement, etc
		const char* Source;

		std::float_t Throughput;
		std::float_t Latency;
		std::size_t Uops;
		const char* Ports;
	};

	struct ArchitectureLatency
	{
		const char* ArchitectureName;
		std::vector<Latency> Latencies;
	};

	struct Instruction
	{
		const char* Name;
		const char* Operands;
		std::vector<ArchitectureLatency> ArchLatency;
	};

	std::vector<Instruction> Instructions;
};
}