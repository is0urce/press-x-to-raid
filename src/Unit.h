// name: Unit.h
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

#include "Point.h"
#include "Appearance.h"

namespace px
{
	namespace ui
	{
		class Gui;
	}
	namespace vr
	{
		class IEffect;
		class Actor;
		class Unit
		{
		public:
			typedef std::string Name;
			typedef int Faction;
			typedef Actor User;
			typedef std::function<void()> Perception;

		private:
			Appearance _appearance;
			Point _position;
			Point _prev;
			bool _remove;
			Name _name;
			int _faction;
			Perception m_on_perception;

			// ctor & dtor
		public:
			Unit();
			Unit(const Point& position);
			virtual ~Unit();
		private:
			Unit(const Unit&);

			// vitrual
		protected:
			virtual bool IsTraversableUnit() const;
			virtual bool IsTransparentUnit() const;
			virtual void TickUnit(int delta);
			virtual void ApplyEffectUnit(IEffect &effect);
			virtual void ExamineUnit(ui::Gui &table, const Point &start, const Point &range) const;
			virtual void UseUnit(User &user);

		public:
			bool IsTraversable();
			bool IsTransparent();
			void Tick(int delta);
			void ApplyEffect(IEffect &effect);
			void Examine(ui::Gui &table, const Point &start, const Point &range) const;
			void Use(User &user);

			void Remove();
			bool IsDestroying() const;

			// position
			const Point& GetPosition() const;
			const Point& GetPreviousPosition() const;
			void SetPosition(const Point& position);
			void SetPosition(const Point& position, const Point& prev);
			void StorePosition();

			// appearance
			const Appearance& GetAppearance() const;
			void SetAppearance(const Appearance& appearance);
			void SetName(Name name);
			Name GetName() const;

			void SetFaction(Faction faction);
			int GetReputation(const Unit& unit) const;
			void SetPerceptionDelegate(Perception on_perception);
		};
	}
}
