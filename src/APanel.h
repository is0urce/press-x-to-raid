// name: APanel.h
// type: c++
// desc: abstract class declaration
// auth: isource

#pragma once

#include "Point.h"
#include "IControl.h"

namespace px
{
	namespace ui
	{
		class Gui;

		class APanel : public IControl
		{
		private:
			bool _visible;
			Gui* _table;

		public:
			APanel(Gui* table);
			virtual ~APanel();

		protected:
			virtual bool HoverControl(int x, int y);
			virtual bool ClickControl(int x, int y, int button);
			virtual bool KeyControl(int key);
			virtual bool ScrollControl(int delta);
			virtual void DrawPanel() = 0;

		public:
			Gui* GetTable();
			bool IsEnabled() const;
			void SetEnabled(bool visible);
			void Enable();
			void Disable();
			void Toggle();
			void Draw();
			void Resize(int x, int y);
		};
	}
}