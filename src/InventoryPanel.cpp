// name: InventoryPanel.cpp
// type: c++
// desc: class implementation
// auth: is0urce

#include "stdafx.h"

#include "InventoryPanel.h"
#include "Gui.h"
#include "Game.h"

#include "Actor.h"
#include "Item.h"

#include "EquipmentSlot.h"
#include "Attribute.h"

using namespace px;
using namespace px::ui;
using namespace px::mech;

// helpers

namespace
{
	inline std::string attribute_description(Attribute stat)
	{
		switch(stat)
		{
		case Attribute::strength:
			return "strength";
		case Attribute::intelligence:
			return "intelligence";
		case Attribute::dexterity:
			return "dexterity";

		case Attribute::damage:
			return "damage";
		case Attribute::armor:
			return "armor";

		case Attribute::vitality:
			return "vitality";
		case Attribute::dodge_rating:
			return "dodge";
		case Attribute::resist_rating:
			return "resist";

		case Attribute::critical_rating:
			return "critical";
		case Attribute::accuracy_rating:
			return "accuracy";
		case Attribute::mastery_rating:
			return "mastery";

			//critical_bonus,
			//accuracy_bonus,
			//mastery_bonus,
		default:
			throw std::runtime_error("std::string AttributeDescription(Attribute stat) stat out of range");
			return "";
		}
	}
}

// ctor & dtor

InventoryPanel::InventoryPanel(Gui* gui, mech::Game* game)
    : APanel(gui),
	  _game(game),
	  _scroll(0), _items(1), _size(1)
{
}

InventoryPanel::~InventoryPanel()
{
}

// virtual overloads

bool InventoryPanel::KeyControl(int code)
{
	switch (code)
	{
	case 'I':
	case VK_SPACE:
	case VK_ESCAPE:
	case VK_RETURN:
		_scroll = 0;
		Disable();
		return true;
	}
	return true;
}

bool InventoryPanel::HoverControl(int x, int y)
{
	Gui &table = *GetTable();
	auto player = _game->GetPlayer();
	if (!player) return false;

	int w = table.GetWidth();
	int h = table.GetHeight();

	Point inventory_range(w / 3, h - 2);
	Point inventory_pos(w / 3, 1);

	Point item_range(inventory_range.X - 2, 3);
	Point item_pos = inventory_pos.Moved(Point(1, 1));

	_hover.reset();
	int num = 0;
	int select = -1;
	player->EnumerateInventory([&] (std::shared_ptr<vr::Item> item) -> bool
	{
		if (Point(x, y).InRange(item_pos, item_range))
		{
			select = num + _scroll;
		}
		if (select == num)
		{
			_hover = item;
			return false;
		}
		++num;

		item_pos.Y += item_range.Y;
		return true;
	});

	return true;
}

bool InventoryPanel::ClickControl(int x, int y, int button)
{
	Gui &table = *GetTable();
	auto player = _game->GetPlayer();
	if (!player) return false;

	int w = table.GetWidth();
	int h = table.GetHeight();

	Point inventory_range(w / 3, h - 2);
	Point inventory_pos(w / 3, 1);

	if (!Point(x, y).InRange(inventory_pos, inventory_range))
	{
		_scroll = 0;
		Disable();
		return true;
	}

	Point item_range(inventory_range.X - 2, 3);
	Point item_pos = inventory_pos.Moved(Point(1, 1));

	int num = 0;
	int select = -1;
	player->EnumerateInventory([&] (std::shared_ptr<vr::Item> item) -> bool
	{
		if (Point(x, y).InRange(item_pos, item_range))
		{
			select = num + _scroll;
		}
		if (select == num)
		{
			player->Equip(item);
			return false;
		}

		++num;
		item_pos.Y += item_range.Y;
		return true;
	});

	return true;
}

bool InventoryPanel::ScrollControl(int delta)
{
	_scroll += delta < 0 ? 1 : (delta > 0 ? -1 : 0);
	_scroll = (std::min)(_scroll, _items - _size);
	_scroll = (std::max)(0, _scroll);
	return true;
}

void InventoryPanel::DrawPanel()
{
	Gui &table = *GetTable();
	auto player = _game->GetPlayer();
	if (!player) return;

	int w = table.GetWidth();
	int h = table.GetHeight();

	auto weapon = player->GetEquipment(EquipmentSlot::weapon);
	auto armor = player->GetEquipment(EquipmentSlot::armor);

	Point inventory_range(w / 3, h - 2);
	//inventory_range.Y = inventory_range.Y / 3 * 3 + 2;
	Point inventory_pos(w / 3, 1);

	Point equipment_range(w / 3 - 2, 9);
	Point weapon_panel(1, 1);
	Point armor_panel = weapon_panel.Moved(Point(0, equipment_range.Y + 1));
	Point hover_panel = Point(inventory_pos.X + inventory_range.X + 1, weapon_panel.Y);

	Color color_border(0x6b839c);
	Color color_normal(0x2b4c6f);
	Color color_select(0x052341);
	Color color_text(0xffffff);
	color_border.A = 0.5;
	color_normal.A = 0.5;
	color_select.A = 0.5;

	// inventory
	Point item_range(inventory_range.X - 2, 3);
	Point item_pos = inventory_pos.Moved(Point(1, 1));
	table.Rectangle(inventory_pos, inventory_range, color_border);
	_items = 0;
	_size = 0;
	player->EnumerateInventory([&] (std::shared_ptr<vr::Item> item) -> bool
	{
		if (_scroll >= ++_items) return true;
		if (item_pos.Y + item_range.Y >= h) return true;
		++_size;

		table.Rectangle(item_pos, item_range, weapon == item || armor == item ? color_select : color_normal);
		table.SetTextColor(color_text);
		table.Write(item_pos.Moved(Point(2, 1)), item->GetName());
		item_pos.Y += item_range.Y;
		return true;
	});

	// weapon
	table.Rectangle(weapon_panel, equipment_range, color_border);
	table.Rectangle(weapon_panel, Point(equipment_range.X, 3), color_select);
	std::stringstream ss;
	ss << "Weapon: ";
	if (weapon)
	{
		ss << weapon->GetName();
	}
	else
	{
		ss << "None";
	}
	table.SetTextColor(color_text);
	table.Write(weapon_panel.Moved(Point(1, 1)), ss.str());
	Point ench_pos = weapon_panel.Moved(Point(1, 4));
	if (weapon)
	{
		weapon->EnumerateEnhancements([&] (const mech::Enhancement &enchant) -> bool
		{
			std::stringstream ss;
			ss << "+" << enchant.GetValue() << " " << attribute_description(enchant.GetAttribute());
			table.Write(ench_pos, ss.str());
			ench_pos.Y += 1;
			return true;
		});
	}

	// armor
	table.Rectangle(armor_panel, equipment_range, color_border);
	table.Rectangle(armor_panel, Point(equipment_range.X, 3), color_select);
	ss.str("");
	ss << "Armor: ";
	if (armor)
	{
		ss << armor->GetName();
	}
	else
	{
		ss << "None";
	}
	table.SetTextColor(color_text);
	table.Write(armor_panel.Moved(Point(1, 1)), ss.str());
	ench_pos = armor_panel.Moved(Point(1, 4));
	if (armor)
	{
		armor->EnumerateEnhancements([&] (const mech::Enhancement &enchant) -> bool
		{
			ss.str("");
			ss << "+" << enchant.GetValue() << " " << attribute_description(enchant.GetAttribute());
			table.Write(ench_pos, ss.str());
			ench_pos.Y += 1;
			return true;
		});
	}

	// hover
	auto hover = _hover.lock();
	if (hover)
	{
		table.Rectangle(hover_panel, equipment_range, color_border);
		table.Rectangle(hover_panel, Point(equipment_range.X, 3), color_select);
		ss.str("");
		ss << hover->GetName();
		table.SetTextColor(color_text);
		table.Write(hover_panel.Moved(Point(1, 1)), ss.str());
		ench_pos = hover_panel.Moved(Point(1, 4));
		if (hover)
		{
			hover->EnumerateEnhancements([&] (const mech::Enhancement &enchant) -> bool
			{
				ss.str("");
				ss << "+" << enchant.GetValue() << " " << attribute_description(enchant.GetAttribute());
				table.Write(ench_pos, ss.str());
				ench_pos.Y += 1;
				return true;
			});
		}
	}
}