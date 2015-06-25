#pragma once

#include "Point.h"
#include "Color.h"

namespace px
{
	namespace vr
	{
		template <typename _S>
		class Notification
		{
		private:
			_S _text;
			Point _point;
			Color _color;

		public:
			Notification(Point point, Color color, _S text) : _point(point), _color(color), _text(text)
			{
			}
			~Notification()
			{
			}

		public:
			Point GetPosition() const
			{
				return _point;
			}
			Color GetColor() const
			{
				return _color;
			}
			_S GetText() const
			{
				return _text;
			}
		};
	}
}