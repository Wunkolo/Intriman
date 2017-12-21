#pragma once
#include "../Intrinsic.hpp"

namespace Intriman
{
class IGenerator
{
public:
	IGenerator()
	{
	}

	virtual ~IGenerator()
	{
	}

	virtual void StartList(const char* Version, const char* Date)
	{
	}

	virtual void VisitIntrinsic(const Intrinsic& CurIntrin)
	{
	}

	virtual void EndList()
	{
	}
};
}
