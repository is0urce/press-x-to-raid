// name: MainPanel.cpp
// type: c++
// desc: class implementation
// auth: is0urce

#include "stdafx.h"

#include "MainPanel.h"

#include "MenuPanel.h"
#include "InventoryPanel.h"
#include "SkillPanel.h"
#include "Game.h"
#include "Gui.h"
#include "Actor.h"
#include "Ability.h"
#include "Unit.h"

#include "Attribute.h"

using namespace px;
using namespace px::ui;
using namespace px::mech;

// helpers

namespace
{
	inline Color select_color(Attribute stat)
	{
		switch (stat)
		{
		case Attribute::strength:
			return 0xaa6539;
		case Attribute::intelligence:
			return 0x2b4c6f;
		case Attribute::dexterity:
			return 0x328a2e;
		default:
			break;
		}
		return 0xffffff;
	}
}

// 

MainPanel::MainPanel(Gui* gui, Game* game)
	: APanel(gui),
	_game(game),
	_menu(new MenuPanel(gui, game)),
	_inventory(new InventoryPanel(gui, game)),
	_skills(new SkillPanel(gui, game))
{
	_inventory->Disable();
	_skills->Disable();
}

MainPanel::~MainPanel()
{
}

// virtual overloads

bool MainPanel::KeyControl(int code)
{
	if (!_game->GetPlayer()) return _menu->Key(code);

	if (_inventory->IsEnabled())
	{
		return _inventory->Key(code);
	}
	if (_skills->IsEnabled())
	{
		return _skills->Key(code);
	}

	switch (code)
	{
	case 'X':
		if (_game->GetPlayer()->IsDead())
		{
			_game->Start(1);
		}
		break;
	case 'I':
		_inventory->Enable();
		return true;
	case VK_F1:
		_skills->Select(1);
		return true;
	case VK_F2:
		_skills->Select(2);
		return true;
	case VK_F3:
		_skills->Select(3);
		return true;
	case VK_F4:
		_skills->Select(4);
		return true;
	default:
		break;
	}

	return false;
}

bool MainPanel::HoverControl(int x, int y)
{
	if (!_game->GetPlayer()) return _menu->Hover(x, y);

	if (_inventory->IsEnabled())
	{
		return _inventory->Hover(x, y);
	}
	if (_skills->IsEnabled())
	{
		return _skills->Hover(x, y);
	}

	// skills
	_ability = -1;
	Gui &table = *GetTable();
	auto player = _game->GetPlayer();
	int w = table.GetWidth();
	int h = table.GetHeight();
	Point skills_pos(1, h - 2);
	Point button_size(13, 1);
	int skills_total = 5;
	for (int i = 1; i < skills_total; ++i)
	{
		if (player->GetSkill(i) && Point(x, y).InRange(skills_pos, button_size))
		{
			_ability = i;
			return true;
		}
		skills_pos.Move(Point(button_size.X + 1, 0));
	}

	return false;
}

bool MainPanel::ClickControl(int x, int y, int button)
{
	if (!_game->GetPlayer()) return _menu->Click(x, y, button);

	if (_inventory->IsEnabled())
	{
		return _inventory->Click(x, y, button);
	}
	if (_skills->IsEnabled())
	{
		return _skills->Click(x, y, button);
	}

	// skills
	Gui &table = *GetTable();
	auto player = _game->GetPlayer();
	int w = table.GetWidth();
	int h = table.GetHeight();
	Point skills_pos(1, h - 2);
	Point button_size(13, 1);
	int skills_total = 5;
	for (int i = 1; i < skills_total; ++i)
	{
		if (player->GetSkill(i))
		{
			if (Point(x, y).InRange(skills_pos, button_size))
			{
				_skills->Select(i);
				return true;
			}
		}
		skills_pos.Move(Point(button_size.X + 1, 0));
	}

	return false;
}

bool MainPanel::ScrollControl(int delta)
{
	if (!_game->GetPlayer()) return _menu->Scroll(delta);

	if (_inventory->IsEnabled())
	{
		return _inventory->Scroll(delta);
	}
	if (_skills->IsEnabled())
	{
		return _skills->Scroll(delta);
	}

	return false;
}

void MainPanel::DrawPanel()
{	
	Gui &table = *GetTable();
	table.SetDrawColor(Color(0, 0, 0, 0));
	table.ClearScreen();

	if (!_game->GetPlayer()) return _menu->Draw();

	if (_inventory->IsEnabled())
	{
		_inventory->Draw();
		return;
	}
	if (_skills->IsEnabled())
	{
		_skills->Draw();
		return;
	}

	int w = table.GetWidth();
	int h = table.GetHeight();

	Point status_size(13, 5);
	Point player_pos(3, 3);
	Point target_pos(w - player_pos.X - status_size.X, player_pos.Y);

	// player
	auto player = _game->GetPlayer();
	player->Examine(table, player_pos, status_size);

	// target
	auto target = _game->GetTarget();
	if (target)
	{
		target->Examine(table, target_pos, status_size);
	}

	// skills
	Point skills_pos(3, h - 2);
	Point button_size(13, 1);
	int skills_total = 5;

	for (int i = 1; i < skills_total; ++i)
	{
		auto skill = player->GetSkill(i);

		if (skill)
		{
			bool useable = true;
			useable = skill->IsTargeted() ? skill->CanUse(target) : skill->CanUse(_game->GetHoverPoint());
			table.Rectangle(skills_pos, button_size, useable ? select_color(skill->GetSpecialisation()) : 0x333333);
			table.SetTextColor(0x000000);
			table.Write(skills_pos.Moved(Point(1, 0)), skill->GetName());
		}

		skills_pos.Move(Point(button_size.X + 1, 0));
	}

	// skill desc
	Point desc_pos(3, skills_pos.Y - 7);
	mech::Ability* skill = nullptr;
	if (_ability >= 0 && (skill = player->GetSkill(_ability)))
	{
		table.SetTextColor(0xffffff);
		table.Write(desc_pos.Moved(Point(0, 0)), skill->GetName());
		table.Write(desc_pos.Moved(Point(0, 1)), skill->GetDescription());

		std::stringstream ss;
		ss << "CD: " << skill->GetCooldownRemain() << " / " << skill->GetCooldown();

		table.Write(desc_pos.Moved(Point(0, 2)), ss.str());

		ss.str("");
		ss << "Cost: " << skill->GetCost();
		table.Write(desc_pos.Moved(Point(0, 3)), ss.str());

		ss.str("");
		ss << "Range: " << skill->GetRange();
		table.Write(desc_pos.Moved(Point(0, 4)), ss.str());
	}

	if (player->IsDead())
	{
		table.Rectangle(Point(0, 0), Point(w - 1, h - 1), Color(0, 0, 0, 0.5));
		table.SetTextColor(0xffffff);
		table.Write(Point(w / 2 - 5, h / 2 - 1), "You are dead");
		table.Write(Point(w / 2 - 12, h / 2 + 1), "Press X to accept your fate");
	}
}