// name: IControl.h
// type: c++ header
// desc: interface declaration
// auth: is0urce

// intefcace for control in main mvc pattern
// functions return true if interaction handled

#pragma once

namespace px
{
	namespace ui
	{
		class IControl
		{
		protected:
			virtual bool KeyControl(int code) = 0;
			virtual bool HoverControl(int x, int y) = 0;
			virtual bool ClickControl(int x, int y, int button) = 0;
			virtual bool ScrollControl(int delta) = 0;

		public:
			bool Key(int code) { return KeyControl(code); }
			bool Hover(int x, int y) { return HoverControl(x, y); }
			bool Click(int x, int y, int button) { return ClickControl(x, y, button); }
			bool Scroll(int delta) { return ScrollControl(delta); }

			virtual ~IControl() {}
		};
	}
}