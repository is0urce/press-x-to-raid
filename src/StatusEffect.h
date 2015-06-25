// name: StatusEffect.h
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

#include "Status.h"
#include "IEffect.h"

namespace px
{
	namespace vr
	{
		class Actor;
	}
	namespace mech
	{
		class StatusEffect : public Status<vr::Actor>, public vr::IEffect
		{
		public:
			StatusEffect(Status status);
			virtual ~StatusEffect();

		protected:
			virtual void ApplyEffect(vr::Actor &actor);
		};
	}
}