// name: Weapon.h

#pragma once

#include "Item.h"

namespace px
{
	namespace vr
	{
		class Weapon : public Item
		{
		public:
			Weapon();
			virtual ~Weapon();

		protected:
			virtual bool EquipItem(Actor &actor);
		};
	}
}
