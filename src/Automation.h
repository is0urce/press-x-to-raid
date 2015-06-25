// name: Automation.h
// type: c++ header
// desc: template class for cellular automation
// auth: is0urce

#pragma once

#include "Map.h"

namespace px
{
	template<typename _C>
	class Automation : public Map<_C>
	{
	public:
		Automation(const Point& range) : Map(range) {}
		~Automation() {}

	private:
		_C& GetCell(Point position)
		{
			return GetElement(position.Clamped(Point::Zero(), GetRange().Moved(Point(-1, -1))));
		}
	public:
		template<typename _A>
		void Execute(std::function<_A(_A, const _C& element)> fold, _A start, std::function<_C(_A)> select, int count)
		{
			for (int i = 0; i < count; ++i)
			{
				std::unique_ptr<Map<_C>> map(new Map<_C>(GetRange()));
				Point::EnumerateRectangle(GetRange(), [&] (const Point &p)
				{
					_A acc = start;
					acc = fold(acc, GetCell(p));
					acc = fold(acc, GetCell(p.Moved(Point( 0,  1))));
					acc = fold(acc, GetCell(p.Moved(Point( 0, -1))));
					acc = fold(acc, GetCell(p.Moved(Point(-1,  0))));
					acc = fold(acc, GetCell(p.Moved(Point( 1,  0))));
					acc = fold(acc, GetCell(p.Moved(Point( 1,  1))));
					acc = fold(acc, GetCell(p.Moved(Point(-1, -1))));
					acc = fold(acc, GetCell(p.Moved(Point( 1, -1))));
					acc = fold(acc, GetCell(p.Moved(Point(-1,  1))));
					map->GetElement(p) = select(acc);
				});
				SwapMaps(*map);
			}
		}
		template<typename _A>
		void Execute(std::function<_A(_A, const _C& element)> fold, _A start, std::function<_C(_A)> select)
		{
			Execute(fold, start, select, 1);
		}
	};
}

