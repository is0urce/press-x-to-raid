// name: Ability.h
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

#include "AbilityDescriptor.h"
#include "IAbility.h"

namespace px
{
	namespace vr
	{
		class Actor;
		class Unit;
	}
	namespace mech
	{
		class Ability : 
			public AbilityDescriptor<std::shared_ptr<vr::Actor>, std::shared_ptr<vr::Unit>>,
			public IAbility<std::shared_ptr<vr::Unit>>
		{
		public:
			typedef std::shared_ptr<vr::Actor> User;
			typedef std::shared_ptr<vr::Unit> Target;
			typedef AbilityDescriptor<User, Target> Descriptor;

		private:
			User _user;
			int _cd;

		public:
			Ability(AbilityDescriptor<User, Target>, User unit);
			virtual ~Ability();

		protected:
			virtual void UseAbility(Target target);
			virtual void UseAbility(const Point& target);
			virtual bool CanUseAbility(Target target) const;
			virtual bool CanUseAbility(const Point& target) const;
			virtual bool IsTargetedAbility() const;

		public:
			int GetCooldownRemain() const;
			void Cooldown(int turns);
		};

	}
}