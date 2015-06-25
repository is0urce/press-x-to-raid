// name: MenuPanel.cpp
// type: c++
// desc: class implementation
// auth: is0urce

#include "stdafx.h"

#include "MenuPanel.h"

#include "Game.h"
#include "Gui.h"
#include "windowsx.h"

namespace
{
	const int button_size = 8;
	const int total_options = 3;
}

using namespace px;
using namespace px::ui;
using namespace px::mech;

MenuPanel::MenuPanel(Gui* gui, Game* game) : APanel(gui), _game(game), _option(0)
{
}

MenuPanel::~MenuPanel()
{
}

// virtual overloads

bool MenuPanel::KeyControl(int code)
{
	switch (code)
	{
	case VK_NUMPAD8:
	case VK_UP:
	case 'W':
		_option--;
		break;
	case VK_NUMPAD2:
	case VK_DOWN:
	case 'S':
		_option++;
		break;
	case VK_NUMPAD5:
	case VK_RETURN:
	case VK_EXECUTE:
	case VK_SPACE:
		switch (_option)
		{
		case 0:
			_game->Start(0);
			break;
		case 1:
			_game->Start(1);
			break;
		default:
			_game->End();
			break;
		}
		break;
	case VK_ESCAPE:
		_game->End();
		break;
	case 'X':
		_game->Start(0);
		break;
	default:
		break;
	}

	if (_option < 0)
	{
		_option = total_options - 1;
	}
	if (_option >= total_options)
	{
		_option = 0;
	}

	return true;
}

bool MenuPanel::HoverControl(int x, int y)
{
	int option = GetOption(Point(x, y));
	if (option >= 0)
	{
		_option = option;
	}
	return true;
}

bool MenuPanel::ClickControl(int x, int y, int bu)
{
	int option = GetOption(Point(x, y));
	if (option >= 0)
	{
		_option = option;
		return KeyControl(VK_RETURN);
	}
	return true;
}

bool MenuPanel::ScrollControl(int delta)
{
	_option += delta > 0 ? -1 : 1;
	if (_option < 0)
	{
		_option = total_options - 1;
	}
	if (_option >= total_options)
	{
		_option = 0;
	}
	return true;
}

void MenuPanel::DrawPanel()
{
	Gui *table = GetTable();

	int w = table->GetWidth();
	int h = table->GetHeight();

	table->Rectangle(Point(0, 0), Point(w - 1, h - 1), Color(0, 0, 0, 1));

	int column = 2;
	int line = 2;

	table->SetTextColor(0xffffff);

	table->Write(Point(column, ++line), L"  20 мая 15 года эры данных");
	table->Write(Point(column, ++line), L"  Не уснуть. Которую ночь на стыке сна и бодрствования");
	table->Write(Point(column, ++line), L"вновь чудится та библиотека. Все книги уже написаны");
	table->Write(Point(column, ++line), L"и хранятся в ней. Все знания, все душевные порывы.");
	++line;
	table->Write(Point(column, ++line), L"  26 мая 15 года эры данных");
	table->Write(Point(column, ++line), L"  Светло. Будто бы бродил меж стеллажей");
	table->Write(Point(column, ++line), L"и искал что-то важное,");
	table->Write(Point(column, ++line), L"только что? Осталось чувство, что она где-то далеко");
	table->Write(Point(column, ++line), L"в джунглях, под землёй. Скорей бы спать.");
	++line;
	table->Write(Point(column, ++line), L"  Может она и впрямь существует –");
	table->Write(Point(column, ++line), L"до чего яркими стали ночные видения.");
	table->Write(Point(column, ++line), L"Стеллажи, и ещё - подземные ходы под ней,");
	table->Write(Point(column, ++line), L"разбросанные книги.");
	table->Write(Point(column, ++line), L"Зовёт, ища новых хозяев, взамен пропавших.");
	++line;
	table->Write(Point(column, ++line), L"  Магия. Тайна философского камня. Секрет бессмертия.");
	table->Write(Point(column, ++line), L"Разбросанные по углам пещеры книги. Всего этого нет.");
	++line;
	table->Write(Point(column, ++line), L"  Я смогу. Я видел всё. Ещё глубже - подземные озёра.");
	table->Write(Point(column, ++line), L"Альковы с апокрифами в толще льда.");
	table->Write(Point(column, ++line), L"И я знаю, как туда попасть. Высплюсь потом.");
	++line;
	table->Write(Point(column, ++line), L"  Пещера в джунглях. Лестница, оканчивающаяся ");
	table->Write(Point(column, ++line), L"каменной дверью. Назад пути нет.");
	table->Write(Point(column, ++line), "п»їРћСЃС‚Р°Р»РѕСЃСЊ РєР°Рє СЃР»РµРґСѓРµС‚ РЅР°Р¶Р°С‚СЊ.");

	DrawButton(0, "РџСЂРѕР»РѕРі");
	DrawButton(1, "Р РµСЃС‚Р°СЂС‚");
	DrawButton(2, "Р’С‹С…РѕРґ");

	std::wstring title = L"НАЖМИТЕ ИКС ЧТОБЫ РАЗГРАБИТЬ БИБЛИОТЕКУ";

	table->Write(Point(w / 2 - title.length() / 2, h - 2), title);
}

void MenuPanel::DrawButton(int num, const std::string &label)
{
	Gui *table = GetTable();

	Point position = GetButtonPosition(num);
	if (_option == num)
	{
		table->Write(position, L"> ");
	}
	position.Move(Point(2, 0));

	std::string text;

	table->Write(position, label);
}

Point MenuPanel::GetButtonPosition(int option)
{
	auto *table = GetTable();
	int w = table->GetWidth();
	int h = table->GetHeight();

	int column = w * 3 / 4 + 1;
	int line = h / 2;
	return Point(column, line + option * 2);
}

int MenuPanel::GetOption(Point point)
{
	auto *table = GetTable();
	int w = table->GetWidth();
	int h = table->GetHeight();

	int column = w * 3 / 4 + 1;
	int line = h / 2;

	if (point.X < column || point.X >= column + button_size) return -1;

	int select = point.Y - line;

	if (select % 2 == 1) return -1;
	select /= 2;

	if (select >= total_options) return -1;

	return select;
}