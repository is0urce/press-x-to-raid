// name: IEffect.cpp
// type: c++
// desc: interface methods
// auth: is0urce

#include "stdafx.h"

#include "IEffect.h"

using namespace px;
using namespace px::vr;

IEffect::~IEffect() // must have, even with = 0
{
}

void IEffect::Apply(Actor &unit)
{
	ApplyEffect(unit);
}
