// name: Item.h
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

#include "Unit.h"
#include "Enhancement.h"

namespace px
{
	namespace vr
	{
		class Actor;

		class Item : public Unit
		{
		private:
			std::list<mech::Enhancement> _enhancements;

		public:
			Item();
			virtual ~Item();

		protected:
			virtual bool EquipItem(Actor &actor);

		public:
			bool Equip(Actor& actor);
			void Enchant(mech::Enhancement enhancement);
			void Enhance(mech::Statistics &stats) const;
			void EnumerateEnhancements(std::function<bool(const mech::Enhancement&)>) const;
		};

	}
}