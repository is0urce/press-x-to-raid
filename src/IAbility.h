// name: IAbility.h
// type: c++ header
// desc: interface class declaration
// auth: is0urce

#pragma once

#include "Point.h"

namespace px
{
	namespace vr
	{
		class Unit;
		class Scene;
	}

	namespace mech
	{
		template <typename _T>
		class IAbility
		{
		public:
			virtual ~IAbility() = 0 {}

		protected:
			virtual void UseAbility(_T target) = 0;
			virtual void UseAbility(const Point& target) = 0;
			virtual bool CanUseAbility(_T target) const = 0;
			virtual bool CanUseAbility(const Point& target) const = 0;
			virtual bool IsTargetedAbility() const = 0;

		public:
			bool IsTargeted() const { return IsTargetedAbility(); }
			bool CanUse(_T target) { return CanUseAbility(target); } const
			bool CanUse(const Point& target) { return CanUseAbility(target); } const
			void Use(_T target) { UseAbility(target); }
			void Use(const Point& target) { UseAbility(target); }
		};
	}
}