#pragma once

#include "Unit.h"

#include <memory>

namespace px
{
	namespace mech
	{
		class Game;
	}
	namespace vr
	{
		class Item;
		class Chest : public Unit
		{
		public:
			typedef std::shared_ptr<Item> ItemPtr;

		private:
			mech::Game* _game;
			ItemPtr _item;

		public:
			Chest(mech::Game* game, ItemPtr item);
			~Chest();

		protected:
			virtual void ExamineUnit(ui::Gui &table, const Point &start, const Point &range) const;
			virtual void UseUnit(Unit::User &user);
		};
	}
}

