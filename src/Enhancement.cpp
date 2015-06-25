#include "stdafx.h"

#include "Enhancement.h"

#include "Statistics.h"

using namespace px;
using namespace px::mech;

Enhancement::Enhancement(Attribute stat, int mod) : _index(stat), _modify(mod)
{
}

Enhancement::~Enhancement()
{
}

void Enhancement::Enhance(Statistics &statistics) const
{
	statistics.Enhance(_index, _modify);
}

Attribute Enhancement::GetAttribute() const
{
	return _index;
}
int Enhancement::GetValue() const
{
	return _modify;
}