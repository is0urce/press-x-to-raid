#pragma once

#include "AbilityDescriptor.h"

#include <vector>
#include <functional>

namespace px
{
	namespace vr
	{
		class Actor;
		class Unit;
	}

	namespace mech
	{
		class Game;
		enum class Attribute : int;

		class AbilityDirectory
		{
		public:
			typedef std::shared_ptr<vr::Actor> User;
			typedef std::shared_ptr<vr::Unit> Target;
			typedef AbilityDescriptor<User, Target> Descriptor;

		private:
			std::vector<Descriptor> _skills;

		public:
			AbilityDirectory();
			~AbilityDirectory();

		public:
			void Load(Game &game);
			Descriptor GetDescriptor(int ability_index) const;
			Descriptor GetDescriptor(std::string name) const;
			void EnumAbilities(std::function<bool(int ability_index, const Descriptor &ability)> enum_function) const;
			void EnumAbilities(Attribute specialization, std::function<bool(int ability_index, const Descriptor &ability)> enum_function) const;
		};
	}
}

