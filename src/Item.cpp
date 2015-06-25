// name: Item.cpp
// type: c++
// desc: class implementation
// auth: is0urce

#include "stdafx.h"

#include "Item.h"

#include "Actor.h"

using namespace px;
using namespace px::vr;

// ctor & dtor

Item::Item()
{
}

Item::~Item()
{
}

// virtual

bool Item::EquipItem(Actor &actor)
{
	return false;
}

// methods

bool Item::Equip(Actor &actor)
{
	return EquipItem(actor);
}

void Item::Enchant(mech::Enhancement enhancement)
{
	_enhancements.push_back(enhancement);
}

void Item::Enhance(mech::Statistics &stats) const
{
	for (auto i : _enhancements)
	{
		i.Enhance(stats);
	}
}

void Item::EnumerateEnhancements(std::function<bool(const mech::Enhancement&)>f) const
{
	for (const auto &i : _enhancements)
	{
		if (!f(i)) return;
	}
}