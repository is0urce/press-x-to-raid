#include "stdafx.h"

#include "Armor.h"

#include "Actor.h"
#include "EquipmentSlot.h"

using namespace px;
using namespace px::vr;
using namespace px::mech;

Armor::Armor()
{
}

Armor::~Armor()
{
}

bool Armor::EquipItem(Actor &actor)
{
	return actor.Equip(EquipmentSlot::armor, this);
}