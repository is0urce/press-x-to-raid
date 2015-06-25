#pragma once

#include "Point.h"
#include "Color.h"

namespace px
{
	namespace vr
	{
		template <typename _M>
		class Broadcast
		{
		private:
			_M _message;
			Point _point;

		public:
			Broadcast(Point point, _M message) : _point(point), _message(message)
			{
			}
			~Broadcast()
			{
			}

		public:
			Point GetPosition() const
			{
				return _point;
			}
			_M GetMessage() const
			{
				return _message;
			}
		};
	}
}