// name: Armor.h

#pragma once

#include "Item.h"

namespace px
{
	namespace vr
	{
		class Armor : public Item
		{
		public:
			Armor();
			virtual ~Armor();

		protected:
			virtual bool EquipItem(Actor &actor);
		};
	}
}
