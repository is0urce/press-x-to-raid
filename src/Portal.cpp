#include "stdafx.h"

#include "Portal.h"

#include "Gui.h"
#include "Game.h"

using namespace px;
using namespace px::vr;

Portal::Portal(mech::Game* game) : _game(game)
{
}

Portal::~Portal()
{
}

void Portal::ExamineUnit(ui::Gui &table, const Point &start, const Point &range) const
{
	Point nameplate = start.Moved(Point(1, 0));
	// name
	table.SetTextColor(Color(1, 1, 1));
	table.Write(nameplate, "HOLE");
	table.Write(nameplate.Moved(Point(0, 2)), "Press E");
	table.Write(nameplate.Moved(Point(0, 3)), "to go deeper");
}

void Portal::UseUnit(Unit::User &user)
{
	_game->NextLevel();
}