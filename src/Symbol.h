// name: Symbol.h
// type: c++ header
// desc: struct definition
// auth: is0urce

#include "Color.h"

namespace px
{
	namespace ui
	{
		struct Symbol
		{
		public:
			int plus;
			Color color;
			Color back;

		public:
			Symbol(int u_plus, Color symbol_color, Color background_color) : plus(u_plus), color(symbol_color), back(background_color) {}
			Symbol() : plus(' ') {}
		};
	}
}