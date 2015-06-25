#include "stdafx.h"

#include "Weapon.h"

#include "EquipmentSlot.h"

#include "Actor.h"

using namespace px;
using namespace px::vr;
using namespace px::mech;

Weapon::Weapon()
{
}

Weapon::~Weapon()
{
}

bool Weapon::EquipItem(Actor &actor)
{
	return actor.Equip(EquipmentSlot::weapon, this);
}