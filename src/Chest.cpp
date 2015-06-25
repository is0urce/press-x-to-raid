#include "stdafx.h"

#include "Chest.h"

#include "Gui.h"
#include "Game.h"

#include "Actor.h"
#include "Item.h"

using namespace px;
using namespace px::vr;

Chest::Chest(mech::Game* game, Chest::ItemPtr item) : _game(game), _item(item)
{
}

Chest::~Chest()
{
}

void Chest::ExamineUnit(ui::Gui &table, const Point &start, const Point &range) const
{
	Point nameplate = start.Moved(Point(1, 0));
	// name
	table.SetTextColor(Color(1, 1, 1));
	table.Write(nameplate, "Chest");
	table.Write(nameplate.Moved(Point(0, 2)), "Press E");
	table.Write(nameplate.Moved(Point(0, 3)), "to loot");
}

void Chest::UseUnit(Unit::User &user)
{
	user.Collect(_item);
	Remove();

	// broadcast
	std::wstringstream ss;
	std::string name = _item->GetName();
	std::wstring wname(name.begin(), name.end());
	ss << "+ " << wname;
	_game->Broadcast(*this, mech::Game::Message(Color(1, 1, 0), ss.str()));

	// turn
	_game->Step(Point(0, 0));
}