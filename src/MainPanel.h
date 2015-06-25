// name: MainPanel.h
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
		class SkillPanel;

		class MainPanel : public APanel
		{
		private:
			mech::Game *_game;
			std::unique_ptr<APanel> _menu;
			std::unique_ptr<APanel> _inventory;
			std::unique_ptr<SkillPanel> _skills;
			int _ability;

		protected:
			virtual void DrawPanel();
			virtual bool KeyControl(int code);
			virtual bool HoverControl(int x, int y);
			virtual bool ClickControl(int x, int y, int button);
			virtual bool ScrollControl(int delta);

		public:
			MainPanel(Gui* gui, mech::Game* game);
			virtual ~MainPanel();
		};
	}
}