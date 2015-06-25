// name: Actor.h
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

#include "Unit.h"
#include "Bar.h"
#include "StatusEffect.h"
#include "Statistics.h"

namespace px
{
	namespace mech
	{
		enum class Attribute : int;
		enum class EquipmentSlot : int;

		class Ability;
	}

	namespace vr
	{
		class Item;
		class Actor : public Unit
		{
		public:
			typedef std::list<mech::StatusEffect> StatusContainer;
			typedef std::list<std::shared_ptr<Item>> Inventory;
			typedef std::vector<std::shared_ptr<Item>> Equipment;
			typedef std::unique_ptr<mech::Ability> AbilityPtr;
			typedef std::vector<AbilityPtr> Skills;

			// attributes
		private:
			Bar<int> _health;
			Bar<int> _energy;
			mech::Statistics _stats;
			mech::Statistics _calculated;
			StatusContainer _effects;

			Skills _skills;

			Inventory _inventory;
			Equipment _equipment;
			std::shared_ptr<Item> _weapon;
			std::shared_ptr<Item> _armor;

			// ctor & dtor
		public:
			Actor();
			virtual ~Actor();

			// virtual overloads
		protected:
			virtual bool IsTraversableUnit() const;
			virtual void TickUnit(int delta);
			virtual void ApplyEffectUnit(IEffect &effect);
			virtual void ExamineUnit(ui::Gui &table, const Point &start, const Point &range) const;

		private:
			void Recalculate();

		public:
			void AddStatusEffect(mech::StatusEffect effect);
			int GetAttribute(mech::Attribute attribute) const;
			void ModAttribute(mech::Attribute attribute, int mod);
			mech::Ability* GetSkill(int slot);
			void SwapSkill(int slot, AbilityPtr skill);

			// health
			int GetCurrentHealth() const;
			int GetMaximumHealth() const;
			void SetHealth(int health);
			void Damage(int damage);
			void Heal(int heal);
			bool IsDead() const;
			// energy
			int GetCurrentEnergy() const;
			int GetMaximumEnergy() const;
			void SpendEnergy(int spend);
			void RestoreEnergy(int restore);
			void SetEnergy(int energy);

			// inventory
			void EnumerateInventory(std::function<bool(std::shared_ptr<Item>)>) const;
			void Collect(std::shared_ptr<Item> item);
			bool Equip(std::shared_ptr<Item> item);
			bool Equip(mech::EquipmentSlot slot, Item* item);
			std::shared_ptr<Item> GetEquipment(mech::EquipmentSlot slot) const;
		};
	}
}