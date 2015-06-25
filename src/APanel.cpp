// name: APanel.cpp
// type: c++
// desc: abstract class methods implementation
// auth: is0urce

#include "stdafx.h"

#include "APanel.h"
#include "Gui.h"

using namespace px;
using namespace px::ui;

APanel::APanel(Gui* table) : _visible(true), _table(table)
{
}

APanel::~APanel()
{
}

bool APanel::HoverControl(int x, int y)
{
	return false;
}

bool APanel::ClickControl(int x, int y, int button)
{
	return false;
}

bool APanel::KeyControl(int key)
{
	return false;
}

bool APanel::ScrollControl(int delta)
{
	return false;
}

void APanel::Draw()
{
	if (_visible)
	{
		DrawPanel();
	}
}

void APanel::SetEnabled(bool visible)
{
	_visible = visible;
}

void APanel::Enable()
{
	_visible = true;
}

void APanel::Disable()
{
	_visible = false;
}

void APanel::Toggle()
{
	_visible = !_visible;
}

bool APanel::IsEnabled() const
{
	return _visible;
}

void APanel::Resize(int x, int y)
{
	_table->Resize(Point(x, y));
}

Gui* APanel::GetTable()
{
	return _table;
}