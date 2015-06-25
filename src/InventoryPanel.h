// name: InventoryPanel.h
// type: c++
// desc: class declaration
// auth: is0urce

#pragma once

#include "APanel.h"

namespace px
{
	namespace vr
	{
		class Item;
	}
	namespace mech
	{
		class Game;
	}
	namespace ui
	{
		class InventoryPanel : public APanel
		{
		private:
			mech::Game *_game;

			std::weak_ptr<vr::Item> _hover;
			int _scroll; // scroll offset
			int _items; // items in inventory
			int _size; // size of list of visible items

		protected:
			virtual void DrawPanel();
			virtual bool KeyControl(int code);
			virtual bool HoverControl(int x, int y);
			virtual bool ClickControl(int x, int y, int button);
			virtual bool ScrollControl(int delta);

		public:
			InventoryPanel(Gui* gui, mech::Game* game);
			virtual ~InventoryPanel();
		};
	}
}