// name: MenuPanel.h
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
	}
	namespace ui
	{
		class Gui;
		class MenuPanel : public APanel
		{
		private:
			mech::Game *_game;

			int _option;

		private:
			Point GetButtonPosition(int option);
			int GetOption(Point point);
			void DrawButton(int num, const std::string &label);

		public:
			MenuPanel(Gui* gui, mech::Game* game);
			virtual ~MenuPanel();

		protected:
			virtual bool KeyControl(int code);
			virtual bool HoverControl(int x, int y);
			virtual bool ClickControl(int x, int y, int button);
			virtual bool ScrollControl(int delta);
			virtual void DrawPanel();
		};
	}
}