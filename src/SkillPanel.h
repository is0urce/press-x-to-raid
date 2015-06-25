// name: SkillPanel.h
// type: c++
// desc: class declaration
// auth: is0urce

#pragma once

#include "APanel.h"

namespace px
{
	namespace mech
	{
		class Game;
		class Ability;
	}
	namespace ui
	{

		class SkillPanel : public APanel
		{
		private:
			mech::Game* _game;
			int _slot;
			int _desc;
			std::unique_ptr<mech::Ability> _skill;

		public:
			SkillPanel(Gui* table, mech::Game* game);
			~SkillPanel();

			// overloads
		private:
			virtual void DrawPanel();
			virtual bool KeyControl(int code);
			virtual bool HoverControl(int x, int y);
			virtual bool ClickControl(int x, int y, int button);
			virtual bool ScrollControl(int delta);

		public:
			void Select(int slot);
			void Close();
		};

	}
}