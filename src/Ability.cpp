// name: Ability.cpp
// type: c++ code
// desc: class implementation
// auth: is0urce

#include "stdafx.h"

#include "Actor.h"
#include "Ability.h"

using namespace px;
using namespace px::mech;

Ability::Ability(AbilityDescriptor<Ability::User, Ability::Target> ability, User user) : AbilityDescriptor(ability), _user(user)
{
	_cd = 0;
}

Ability::~Ability()
{
}

// VIRTUAL OVERLOADS

bool Ability::IsTargetedAbility() const
{
	return _targeted;
}

bool Ability::CanUseAbility(Target target) const
{
	return _user->GetCurrentEnergy() >= GetCost() && _cd == 0 && _target && target && _targetcheck(_user, target);
}

bool Ability::CanUseAbility(const Point &point) const
{
	return _user->GetCurrentEnergy() >= GetCost() && _cd == 0 && _ground && _groundcheck(_user, point);
}

void Ability::UseAbility(Ability::Target target)
{
	if (CanUseAbility(target))
	{
		_target(_user, target);
		_cd = GetCooldown();
		_user->SpendEnergy(GetCost());
	}
}

void Ability::UseAbility(const Point& place)
{
	if (CanUseAbility(place))
	{
		_ground(_user, place);
		_cd = GetCooldown();
		_user->SpendEnergy(GetCost());
	}
}

// METHODS

void Ability::Cooldown(int turns)
{
	_cd = (std::max)(0, _cd - turns);
}

int Ability::GetCooldownRemain() const
{
	return _cd;
}