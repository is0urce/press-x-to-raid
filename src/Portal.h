#pragma once

#include "Unit.h"

namespace px
{
	namespace mech
	{
		class Game;
	}
	namespace vr
	{
		class Portal : public Unit
		{
		private:
			mech::Game* _game;

		public:
			Portal(mech::Game* game);
			~Portal();

		protected:
			virtual void ExamineUnit(ui::Gui &table, const Point &start, const Point &range) const;
			virtual void UseUnit(Unit::User &user);
		};
	}
}

