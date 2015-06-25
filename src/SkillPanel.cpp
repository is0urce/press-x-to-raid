#include "stdafx.h"
#include "SkillPanel.h"

#include "Gui.h"
#include "Game.h"
#include "AbilityDescriptor.h"
#include "Actor.h"
#include "Ability.h"

#include "Attribute.h"

using namespace px;
using namespace ui;

// helpers

namespace
{
	inline Color select_color(mech::Attribute stat)
	{
		switch (stat)
		{
		case mech::Attribute::strength:
			return 0xaa6539;
		case mech::Attribute::intelligence:
			return 0x2b4c6f;
		case mech::Attribute::dexterity:
			return 0x328a2e;
		default:
			break;
		}
		return 0xffffff;
	}
}

// ctor & dtor

SkillPanel::SkillPanel(Gui* table, mech::Game* game) :
	APanel(table),
	_game(game),
	_desc(-1)
{
}


SkillPanel::~SkillPanel()
{
}

void SkillPanel::DrawPanel()
{
	Gui &table = *GetTable();
	int w = table.GetWidth();
	int h = table.GetHeight();
	auto player = _game->GetPlayer();
	const mech::AbilityDirectory &skills = _game->GetSkills();

	Point start;
	Color spec_color;
	int num = 0;
	auto enum_fn = [&table,&start,&spec_color,&num] (int i, const mech::AbilityDirectory::Descriptor &descriptor) -> bool
	{
		table.Rectangle(start + Point(0, num).Multiplied(1, 3), Point(15, 1), spec_color);
		table.SetTextColor(0);
		table.Write(start + Point(1, 0) + Point(0, num).Multiplied(1, 3), descriptor.GetName());
		++num;
		return true;
	};

	static const int stride = 15 + 2;

	start = Point(5 + stride * 0, 5);
	spec_color = select_color(mech::Attribute::strength);
	num = 0;
	skills.EnumAbilities(mech::Attribute::strength, enum_fn);

	start = Point(5 + stride * 1, 5);
	spec_color = select_color(mech::Attribute::dexterity);
	num = 0;
	skills.EnumAbilities(mech::Attribute::dexterity, enum_fn);

	start = Point(5 + stride * 2, 5);
	spec_color = select_color(mech::Attribute::intelligence);
	num = 0;
	skills.EnumAbilities(mech::Attribute::intelligence, enum_fn);

	start = Point(5 + stride * 3, 5);
	spec_color = select_color(mech::Attribute::vitality);
	num = 0;
	skills.EnumAbilities(mech::Attribute::vitality, enum_fn);

	// current skills
	Point skills_pos(1, h - 2);
	Point button_size(13, 1);
	int skills_total = 5;

	for (int i = 1; i < skills_total; ++i)
	{
		auto skill = player->GetSkill(i);

		if (skill)
		{
			bool useable = true;
			if (i == _slot)
			{
				table.Rectangle(skills_pos.Moved(Point(0, -1)), button_size.Moved(Point(0, 1)), select_color(skill->GetSpecialisation()));
				table.SetTextColor(0x000000);
				table.Write(skills_pos.Moved(Point(1, -1)), skill->GetName());
			}
			else
			{
				table.Rectangle(skills_pos, button_size, select_color(skill->GetSpecialisation()));
				table.SetTextColor(0x000000);
				table.Write(skills_pos.Moved(Point(1, 0)), skill->GetName());
			}
		}
		else
		{
			if (i == _slot)
			{
				table.Rectangle(skills_pos.Moved(Point(0, -1)), button_size.Moved(Point(0, 1)), 0x999999);
				table.SetTextColor(0x000000);
				table.Write(skills_pos.Moved(Point(1, -1)), "N/A");
			}
			else
			{
				table.Rectangle(skills_pos, button_size, 0x999999);
				table.SetTextColor(0x000000);
				table.Write(skills_pos.Moved(Point(1, 0)), "N/A");
			}
		}

		skills_pos.Move(Point(button_size.X + 1, 0));
	}


	// skill desc
	Point desc_pos(3, skills_pos.Y - 7);
	mech::Ability* skill = _skill.get();

	if (_desc >= 0)
	{
		skill = player->GetSkill(_desc);
	}
	if (skill)
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
}

bool SkillPanel::KeyControl(int code)
{
	switch (code)
	{
	case VK_F1:
		Select(1);
		break;
	case VK_F2:
		Select(2);
		break;
	case VK_F3:
		Select(3);
		break;
	case VK_F4:
		Select(4);
		break;
	case VK_SPACE:
	case VK_ESCAPE:
	case VK_RETURN:
		Disable();
		return true;
	}
	return true;
}

bool SkillPanel::HoverControl(int x, int y)
{
	_desc = -1;
	_skill.reset();

	Gui &table = *GetTable();
	int w = table.GetWidth();
	int h = table.GetHeight();
	auto player = _game->GetPlayer();
	const mech::AbilityDirectory &skills = _game->GetSkills();

	// skills

	Point skills_pos(1, h - 2);
	Point button_size(13, 1);
	int skills_total = 5;
	for (int i = 1; i < skills_total; ++i)
	{
		if (player->GetSkill(i) && (Point(x, y).InRange(skills_pos, button_size)))
		{
			_desc = i;
			return true;
		}
		skills_pos.Move(Point(button_size.X + 1, 0));
	}

	Point start;
	int num = 0;
	bool found = false;
	auto enum_fn = [&] (int ability_index, const mech::AbilityDirectory::Descriptor &descriptor) -> bool
	{
		if (Point(x, y).InRange(start + Point(0, num).Multiplied(1, 3), Point(15, 1)))
		{
			_skill.swap(std::unique_ptr<mech::Ability>(new mech::Ability(descriptor, player)));
			found = true;
			return false;
		}
		++num;
		return true;
	};

	static const int stride = 15 + 2;

	start = Point(5 + stride * 0, 5);
	num = 0;
	skills.EnumAbilities(mech::Attribute::strength, enum_fn);

	start = Point(5 + stride * 1, 5);
	num = 0;
	skills.EnumAbilities(mech::Attribute::dexterity, enum_fn);

	start = Point(5 + stride * 2, 5);
	num = 0;
	skills.EnumAbilities(mech::Attribute::intelligence, enum_fn);

	start = Point(5 + stride * 3, 5);
	num = 0;
	skills.EnumAbilities(mech::Attribute::vitality, enum_fn);

	return true;
}
bool SkillPanel::ClickControl(int x, int y, int button)
{
	if (button != 1)
	{
		Close();
		return true;
	}

	Gui &table = *GetTable();
	int w = table.GetWidth();
	int h = table.GetHeight();
	auto player = _game->GetPlayer();
	const mech::AbilityDirectory &skills = _game->GetSkills();

	Point start;
	int num = 0;
	bool found = false;
	auto enum_fn = [&] (int ability_index, const mech::AbilityDirectory::Descriptor &descriptor) -> bool
	{
		if (Point(x, y).InRange(start + Point(0, num).Multiplied(1, 3), Point(15, 1)))
		{
			player->SwapSkill(_slot, std::unique_ptr<mech::Ability>(new mech::Ability(descriptor, player)));
			found = true;

			// clear doubles
			for (int i = 1; i < 5; ++i)
			{
				if (i == _slot) continue;
				auto skill = player->GetSkill(i);
				if (skill && skill->GetName() == descriptor.GetName())
				{
					player->SwapSkill(i, nullptr);
				}
			}

			// pass turn
			_game->Step(Point(0, 0));

			return false;
		}
		++num;
		return true;
	};

	static const int stride = 15 + 2;

	start = Point(5 + stride * 0, 5);
	num = 0;
	skills.EnumAbilities(mech::Attribute::strength, enum_fn);

	start = Point(5 + stride * 1, 5);
	num = 0;
	skills.EnumAbilities(mech::Attribute::dexterity, enum_fn);

	start = Point(5 + stride * 2, 5);
	num = 0;
	skills.EnumAbilities(mech::Attribute::intelligence, enum_fn);

	start = Point(5 + stride * 3, 5);
	num = 0;
	skills.EnumAbilities(mech::Attribute::vitality, enum_fn);

	// skills
	Point skills_pos(1, h - 2);
	Point button_size(13, 1);
	int skills_total = 5;
	for (int i = 1; i < skills_total; ++i)
	{
		//if (player->GetSkill(i))
		//{
		if (Point(x, y).InRange(skills_pos, button_size))
		{
			Select(i);
			return true;
		}
		//}
		skills_pos.Move(Point(button_size.X + 1, 0));
	}

	// not handled - clicked outside - close
	Close();
	return true;
}
bool SkillPanel::ScrollControl(int delta)
{
	return true;
}

void SkillPanel::Select(int slot)
{
	if (_slot == slot)
	{
		Close();
	}
	else
	{
		_slot = slot;
		Enable();
	}
}

void SkillPanel::Close()
{
	_slot = 0;
	Disable();
}