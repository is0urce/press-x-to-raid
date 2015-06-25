// name: Gui.cpp
// type: c++
// desc: class implementation
// auth: is0urce

#include "stdafx.h"

#include "Gui.h"

#include "String.h"

using namespace px;
using namespace px::ui;

namespace
{
	const Point base_range(1, 1);
}

Gui::Gui() : Map<Symbol>(base_range), _front(1, 1, 1, 1.0), _back(0, 0, 0, 0.0)
{
}

Gui::~Gui()
{
}

void Gui::ClearScreen()
{
	Symbol fill;
	fill.plus = ' ';
	fill.color = _front;
	fill.back = _back;
	Fill(fill);
}

void Gui::SetTextColor(Color color)
{
	_front = color;
}

void Gui::SetDrawColor(Color color)
{
	_back = color;
}

void Gui::Rectangle(const Point &start, const Point &range)
{
	Rectangle(start, range, _back);
}

void Gui::Rectangle(const Point &start, const Point &range, const Color& color)
{
	range.EnumerateRange(start, [&] (const Point& point)
	{
		if (InRange(point))
		{
			Symbol &s = GetElement(point);
			s.plus = ' ';
			s.back = color;
		}
	});
}

void Gui::Draw(const Point& point)
{
	if (!InRange(point)) return;
	Draw(point, _back);
}

void Gui::Draw(const Point& point, const Color& color)
{
	if (!InRange(point)) return;
	GetElement(point).back = color;
}

void Gui::Write(Point point, int letter)
{
	if (!InRange(point) || letter <= 0) return;

	Symbol &s = GetElement(point);
	s.plus = letter;
	s.color = _front;
}

void Gui::Write(Point point, const std::string &text)
{
	String::enum_utf8(text, [&] (unsigned int letter)
	{
		Write(point, letter);
		++point.X;
	});
}

void Gui::Write(Point point, const std::wstring &text)
{
	for (int i = 0; true; ++i, ++point.X)
	{
		wchar_t letter = text[i];
		Write(point, letter);
		if (letter == 0) return;
	}
}

void Gui::WriteInteger(Point point, int num)
{
	bool negative = num < 0;
	if (negative)
	{
		num = -num;
	}

	do
	{
		Write(point, '0' + num % 10);
		num /= 10;
		--point.X;
	}
	while (num > 0);

	if (negative)
	{
		Write(point, '-');
	}
}