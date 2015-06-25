// name: IEffect.h
// type: c++ header
// desc: declaration of interface
// auth: is0urce

// forms visitor pattern with conjunction with unit applyeffect virtual method

#pragma once

namespace px
{
	namespace vr
	{
		class Actor;
		class IEffect
		{
			// ctor & dtor
		public:
			virtual ~IEffect() = 0;

			// virtual methods
		protected:
			virtual void ApplyEffect(Actor &actor) = 0;

			// methods
		public:
			void Apply(Actor &unit);
		};
	}
}