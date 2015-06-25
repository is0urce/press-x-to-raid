// name: Unit.cpp
// type: c++
// desc: class implementation
// auth: is0urce

#include "stdafx.h"

#include "Unit.h"

using namespace px;
using namespace px::vr;

// ctor & dtor

Unit::Unit() : _remove(false)
{
}

Unit::Unit(const Point& position) : _position(position), _prev(position), _remove(false)
{
}

Unit::~Unit()
{
}

// virtual methods

bool Unit::IsTraversableUnit() const
{
	return false;
}

bool Unit::IsTransparentUnit() const
{
	return true;
}

void Unit::TickUnit(int delta)
{
}

void Unit::ApplyEffectUnit(IEffect &effect)
{
}

void Unit::ExamineUnit(ui::Gui &table, const Point &start, const Point &range) const
{
}

void Unit::UseUnit(Unit::User &user)
{
}

// methods

const Point& Unit::GetPosition() const
{
	return _position;
}

const Point& Unit::GetPreviousPosition() const
{
	return _prev;
}

void Unit::StorePosition()
{
	_prev = _position;
}

bool Unit::IsTraversable()
{
	return IsTraversableUnit();
}

bool Unit::IsTransparent()
{
	return IsTransparentUnit();
}

void Unit::Tick(int delta)
{
	if (m_on_perception)
	{
		m_on_perception();
	}
	TickUnit(delta);
}

void Unit::ApplyEffect(IEffect &effect)
{
	ApplyEffectUnit(effect);
}

void Unit::Examine(ui::Gui &table, const Point &start, const Point &range) const
{
	ExamineUnit(table, start, range);
}

void Unit::Use(Actor &user)
{
	UseUnit(user);
}

void Unit::SetPosition(const Point& position)
{
	_position = position;
}
void Unit::SetPosition(const Point& position, const Point& prev)
{
	_position = position;
	_prev = prev;
}

const Appearance& Unit::GetAppearance() const
{
	return _appearance;
}

void Unit::SetAppearance(const Appearance& appearance)
{
	_appearance = appearance;
}

void Unit::Remove()
{
	_remove = true;
}

bool Unit::IsDestroying() const
{
	return _remove;
}

void Unit::SetName(Unit::Name name)
{
	_name = name;
}

Unit::Name Unit::GetName() const
{
	return _name;
}

void Unit::SetFaction(Faction faction)
{
	_faction = faction;
}

int Unit::GetReputation(const Unit& unit) const
{
	return _faction == unit._faction ? 100 : -100;
}

void Unit::SetPerceptionDelegate(Perception on_perception)
{
	m_on_perception = on_perception;
}