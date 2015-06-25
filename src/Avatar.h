// name: Avater.h
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

// model view of unit

#include "Appearance.h"
#include "Point.h"

namespace px
{
	namespace shell
	{
		class Avatar
		{
		private:
			Appearance _appearance;
			Point _current;
			Point _previous;

		public:
			Avatar(Appearance appearance, Point position, Point previous) : _appearance(appearance), _current(position), _previous(previous)
			{
			}
			~Avatar() {}

		public:
			const Point& GetPosition() const { return _current; };
			const Point& GetPreviousPosition() const { return _previous; };
			const Appearance& GetAppearance() const { return _appearance; };
		};
	}
}
