// name: IEffect.cpp
// type: c++
// desc: interface methods
// auth: is0urce

#include "stdafx.h"

#include "StatusEffect.h"

#include "Actor.h"

using namespace px;
using namespace px::mech;

StatusEffect::StatusEffect(Status status) : Status<vr::Actor>(status)
{
}

StatusEffect::~StatusEffect()
{
}

void StatusEffect::ApplyEffect(vr::Actor &actor)
{
	actor.AddStatusEffect(*this);
}