// name: Perception.cpp
// type: c++
// desc: class implementation
// auth: is0urce

#include "stdafx.h"

#include "Perception.h"

using namespace px;
using namespace px::shell;

namespace
{
	const Color black_color(0);
}

Perception::Perception(const Point& range) : _appearance(range), _color(new Map<Color>(range)), _colorprev(new Map<Color>(range))
{
}

Perception::~Perception()
{
}

int Perception::GetWidth() const
{
	return _appearance.GetWidth();
}

int Perception::GetHeight() const
{
	return _appearance.GetHeight();
}

bool Perception::InRange(Point point) const
{
	return _appearance.InRange(point);
}

const Appearance& Perception::GetAppearance(Point position) const
{
	return _appearance.GetElement(position);
}

const Color& Perception::GetLight(Point position) const
{
	return _color->GetElement(position);
}

const Color& Perception::GetPreviousLight(Point position) const
{
	Point prev = position + _movement;
	return _colorprev->InRange(prev) ? _colorprev->GetElement(prev) : black_color; // _color->GetElement(position); // to fade in
}

void Perception::SetAppearance(Point position, const Appearance& tile)
{
	_appearance.GetElement(position) = tile;
}

void Perception::SetLight(Point position, const Color& color)
{
	_color->GetElement(position) = color;
}

void Perception::AddUnit(Avatar avatar)
{
	_units.push_back(avatar);
}

void Perception::AddUnit(Appearance appearance, Point position, Point previous)
{
	_units.emplace_back(appearance, position, previous);
}

const Perception::UnitList& Perception::GetUnits() const
{
	return _units;
}

void Perception::AddNotification(Point position, Color color, Perception::NotifyString text)
{
	_notifications.emplace_back(position, color, text);
}

const Perception::NotifyList& Perception::GetNotifications() const
{
	return _notifications;
}

Point Perception::GetMovement() const
{
	return _movement;
}

Point Perception::GetStart() const
{
	return _start;
}

void Perception::SetStart(Point point)
{
	_start = point;
}

std::unique_ptr<Perception> Perception::Reassemble(const Point &movement)
{
	std::unique_ptr<Perception> next(new Perception(_appearance.GetRange()));
	next->_colorprev.swap(_color);
	next->_movement = movement;
	next->_start = _start + movement;
	return next;
}