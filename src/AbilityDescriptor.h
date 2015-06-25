// name: AbilityDescriptor.h
// type: c++ header
// desc: template class
// auth: is0urce

// template for generic skill
// _U - user, _T - target

#pragma once

#include <functional>
#include <string>

namespace px
{
	struct Point;

	namespace mech
	{
		enum class Attribute : int;

		template <typename _U, typename _T>
		class AbilityDescriptor
		{
		public:
			typedef std::function<void(_U, _T)> TargetFunction;
			typedef std::function<bool(_U, _T)> TargetFunctionCheck;
			typedef std::function<void(_U, const Point&)> GroundFunction;
			typedef std::function<bool(_U, const Point&)> GroundFunctionCheck;
			typedef std::string Range;

		private:
			int _cost;
			int _cooldown;
			Range _range;
			Attribute _spec;
			bool _hidden;
			std::string _name;
			std::string _description;

		protected:
			bool _targeted;
			TargetFunction _target;
			TargetFunctionCheck _targetcheck;
			GroundFunction _ground;
			GroundFunctionCheck _groundcheck;

		public:
			AbilityDescriptor(Attribute specialization, TargetFunction tf, TargetFunctionCheck tfc) :
				_cooldown(0), _cost(0),
				_range(""),
				_spec(specialization),
				_hidden(false),
				_targeted(true),
				_target(tf), _targetcheck(tfc) {}
			AbilityDescriptor(Attribute specialization, GroundFunction gf, GroundFunctionCheck gfc) :
				_cooldown(0), _cost(0),
				_range(""),
				_hidden(false),
				_spec(specialization),
				_targeted(false),
				_ground(gf), _groundcheck(gfc) {}
			virtual ~AbilityDescriptor() {}

		public:
			int GetCooldown() const { return _cooldown;	}
			int GetCost() const { return _cost;	}
			Range GetRange() const { return _range; }
			std::string GetName() const { return _name; }
			std::string GetDescription() const { return _description; }
			Attribute GetSpecialisation() const { return _spec; }
			bool IsHidden() const { return _hidden; }

			void SetCost(int cost) { _cost = cost; }
			void SetCooldown(int cooldown) { _cooldown = cooldown; }
			void SetRange(Range range) { _range = range; }
			void SetText(std::string name, std::string description) { _name = name; _description = description; }
			void SetRestrictions(int cd, int cost, int range) { _cooldown = cd; _cost = cost; _range = range; }
			void SetSpecialization(Attribute specialization) { _spec = specialization; }
			void SetHidden() { _hidden = true; }
			void SetHidden(bool hidden) { _hidden = hidden; }
		};
	}
}