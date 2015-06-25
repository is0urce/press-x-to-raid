#include "stdafx.h"

#include "Statistics.h"
#include "Attribute.h"

using namespace px;
using namespace px::mech;

Statistics::Statistics() : _values((int)Attribute::max_attribute + 1)
{
}

Statistics::~Statistics()
{
}

void Statistics::Clear()
{
	std::fill(_values.begin(), _values.end(), 0);
}

bool Statistics::IsValid(Attribute stat)
{
	int index = (int)stat;
	return index >= 0 && stat <= Attribute::max_attribute;
}

void Statistics::Enhance(Attribute stat, int modify)
{
	_values.at((int)stat) += modify;
}

int Statistics::GetValue(Attribute stat) const
{
	return _values.at((int)stat);
}