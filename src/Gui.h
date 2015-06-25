// name: Gui.h
// type: c++ header
// desc: class declaration
// auth: is0urce

// extends GuiTable with specific output functions

#pragma once

#include "Map.h"
#include "Symbol.h"

namespace px
{
	namespace ui
	{
		class Gui : public Map<Symbol>
		{
		private:
			Color _front;
			Color _back;

		public:
			Gui();
			virtual ~Gui();

			void ClearScreen();
			void ClearScreen(const Symbol &fill);
			void SetTextColor(Color color);
			void SetDrawColor(Color color);
			void Rectangle(const Point& start, const Point& range);
			void Rectangle(const Point& start, const Point& range, const Color& color);
			void Draw(const Point& point);
			void Draw(const Point& point, const Color& color);

			void Write(Point point, int uplus);
			void Write(Point point, const std::string &text);
			void Write(Point point, const std::wstring &text);
			void WriteInteger(Point point, int x);
		};
	}
}