// name: Actor.cpp
// type: c++
// desc: class methods implementation
// auth: is0urce

#include "stdafx.h"

#include "Actor.h"

#include "Item.h"
#include "IEffect.h"
#include "Gui.h"
#include "Ability.h"

#include "Attribute.h"
#include "EquipmentSlot.h"

namespace
{
	const int skills_total = 5;
}

using namespace px;
using namespace px::vr;

// ctor & dtor

Actor::Actor() : _equipment((int)mech::EquipmentSlot::max_slot + 1), _skills(skills_total)
{
}

Actor::~Actor()
{
}

// virtual overload

bool Actor::IsTraversableUnit() const
{
	return false;
}

void Actor::TickUnit(int delta)
{
	// cooldown skills
	for (auto &s : _skills)
	{
		if (s)
		{
			s->Cooldown(delta);
		}
	}

	// tick effects
	bool expired = false;
	for (auto s = _effects.begin(); s != _effects.end(); )
	{
		s->OnTick(*this, delta);
		if (s->IsExpired())
		{
			s->OnExpire(*this);
			s = _effects.erase(s);
			expired = true;
		}
		else
		{
			++s;
		}
	}
	if (expired)
	{
		Recalculate();
	}

	// r.i.p
	if (IsDead())
	{
		Remove();
	}
}

void Actor::ApplyEffectUnit(IEffect &effect)
{
	effect.Apply(*this);
}

void Actor::ExamineUnit(ui::Gui &table, const Point &start, const Point &range) const
{
	Point nameplate = start.Moved(Point(1, 0));
	Point hp = start.Moved(Point(1, 2));
	Point mp = start.Moved(Point(1, 4));
	Point effect_position = start.Moved(Point(1, 6));

	// name

	Name name = GetName();
	if (name.length() > 0)
	{
		table.SetTextColor(Color(1, 1, 1));
		table.Write(nameplate, name);
	}

	// bars
	int bar_length = range.X - 2;
	int current_health = GetCurrentHealth();
	int max_health = GetMaximumHealth();
	int current_energy = GetCurrentEnergy();
	int max_energy = GetMaximumEnergy();

	max_health = (max_health == 0) ? 1 : max_health;
	max_energy = (max_energy == 0) ? 1 : max_energy;

	int green = bar_length * current_health / max_health;
	green = (green == 0 && current_health > 0) ? 1 : green;
	green = (green < 0) ? 0 : green;

	int blue = bar_length * current_energy / max_energy;
	blue = (blue == 0 && current_energy > 0) ? 1 : blue;
	blue = (blue < 0) ? 0 : blue;

	table.Rectangle(hp, Point(green, 1), 0x085200);
	table.Rectangle(hp.Moved(Point(green, 0)), Point(bar_length - green, 1), 0x642800);
	table.Rectangle(mp, Point(blue, 1), 0x6b839c);
	table.Rectangle(mp.Moved(Point(blue, 0)), Point(bar_length - blue, 1), 0xf0bf9f);

	table.SetTextColor(0x1f0c00);
	table.WriteInteger(hp.Moved(Point(bar_length - 1, 0)), current_health);
	table.WriteInteger(mp.Moved(Point(bar_length - 1, 0)), current_energy);
	table.Write(hp.Moved(Point(1, 0)), "HP");
	table.Write(mp.Moved(Point(1, 0)), "EP");

	// effects

	table.SetTextColor(Color(1, 1, 1));
	for (auto i = _effects.cbegin(); i != _effects.cend(); ++i)
	{
		table.Write(effect_position, i->GetName());
		effect_position.Y += 2;
	}
}

// methods

void Actor::AddStatusEffect(mech::StatusEffect effect)
{
	effect.OnApply(*this);
	if (effect.GetDuration() > 0)
	{
		_effects.push_back(effect);
	}
	else
	{
		effect.OnExpire(*this);
	}
	Recalculate();
}

void Actor::Heal(int amount)
{
	_health.Restore(amount);
}

void Actor::Damage(int damage)
{
	_health.Spend(damage);
}

int Actor::GetCurrentHealth() const
{
	return _health.GetCurrent();
}

int Actor::GetMaximumHealth() const
{
	return _health.GetMaximum();
}
void Actor::SetHealth(int max)
{
	_health.SetValues(max);
}

bool Actor::IsDead() const
{
	return _health.IsEmpty();
}

int Actor::GetCurrentEnergy() const
{
	return _energy.GetCurrent();
}
int Actor::GetMaximumEnergy() const
{
	return _energy.GetMaximum();
}
void Actor::SpendEnergy(int spend)
{
	_energy.Spend(spend);
}
void Actor::RestoreEnergy(int restore)
{
	_energy.Restore(restore);
}
void Actor::SetEnergy(int energy)
{
	_energy.SetValues(energy);
}

void Actor::Recalculate()
{
	_calculated = _stats;
	for (auto i : _equipment)
	{
		if (i)
		{
			i->Enhance(_calculated);
		}
	}
	
	_health.SetMaximum(100 + _calculated.GetValue(mech::Attribute::vitality) * 5);
}

int Actor::GetAttribute(mech::Attribute stat) const
{
	return _calculated.GetValue(stat);
}

void Actor::ModAttribute(mech::Attribute stat, int mod)
{
	_stats.Enhance(stat, mod);
	Recalculate();
}

void Actor::Collect(std::shared_ptr<Item> item)
{
	if (!item) throw std::runtime_error("Actor::CollectCollect(std::shared_ptr<Item> item) - item is empty");

	_inventory.push_front(item);
}

bool Actor::Equip(std::shared_ptr<Item> item)
{
	return item->Equip(*this);
}

bool Actor::Equip(mech::EquipmentSlot slot, Item* item)
{
	bool success = false;
	for (std::shared_ptr<Item> inventory_item : _inventory)
	{
		if (inventory_item.get() == item)
		{
			_equipment.at((int)slot) = inventory_item;
			success = true;
			break;
		}
	}

	if (success)
	{
		Recalculate();
	}

	return success;
}

std::shared_ptr<Item> Actor::GetEquipment(mech::EquipmentSlot slot) const
{
	return _equipment.at((int)slot);
}

//void Actor::Unequip(Item* item)
//{
//	_weapon = (_weapon == item) ? 0 : _weapon;
//}
//
//void Actor::Drop(Item* item)
//{
//	Unequip(item);
//	_inventory.remove_if([item, this] (const std::unique_ptr<Item> &x) { return x.get() == item; });
//}
//

//const Actor::Inventory& Actor::GetInventory() const
//{
//	return _inventory;
//}

void Actor::EnumerateInventory(std::function<bool(std::shared_ptr<Item> item)> f) const
{
	for (std::shared_ptr<Item> item : _inventory)
	{
		if (!f(item)) return;
	}
}

mech::Ability* Actor::GetSkill(int slot)
{
	return _skills.at(slot).get();
}

void Actor::SwapSkill(int slot, AbilityPtr skill)
{
	_skills.at(slot).swap(skill);
}