// name: Slot.h

#pragma once

namespace px
{
	namespace mech
	{
		enum class EquipmentSlot : int
		{
			weapon,
			armor,

			max_slot = armor
		};
	}
}