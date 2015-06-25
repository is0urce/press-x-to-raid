// name: Map.h
// type: c++ header
// desc: class declaration
// auth: is0ruce

// generic 2-d container

#pragma once

#include "Point.h"

namespace px
{
	template <typename _T>
	class Map
	{
		template<typename _V>
		friend class Map;
	public:
		typedef _T _F;

		// attributes
	private:
		Point _range;
		std::unique_ptr<_T[]> _tiles;

		// ctor & dtor
	public:
		Map(Point range) : _range(range)
		{
			if (range.X <= 0 || range.Y <= 0) throw std::runtime_error("Map<_T>::ctor(Point range) invalid range (x or y <= 0)");
			_tiles.reset(new _T[range.X * range.Y]);
		};
		~Map() {};
	private:
		Map(const Map&); // disable copy

		// methods
	private:
		int GetLength() const { return _range.X * _range.Y; };
	public:
		int GetWidth() const { return _range.X; };
		int GetHeight() const { return _range.Y; };
		const Point& GetRange() const { return _range; };
		bool InRange(const Point& point) const { return point.X >= 0 && point.Y >= 0 && point.X < _range.X && point.Y < _range.Y; };
		void SwapMaps(Map& that)
		{
			std::swap(_range, that._range);
			_tiles.swap(that._tiles);
		}
		void Resize(Point range)
		{
			if (_range == range) return;

			Map map(range);
			SwapMaps(map);
		}
		_T& GetElement(const Point& point, _T& outer) const { return InRange(point) ? _tiles[_range.X * point.Y + point.X] : outer; }
		_T& GetElement(const Point& point) const
		{
			if (!InRange(point)) throw std::runtime_error("_T& Map<_T>::GetElement(Point point) argument out of range");
			return _tiles[_range.X * point.Y + point.X];
		}
		void Fill(const _T &tile)
		{
			int len = GetLength();
			for (int i = 0; i < len; ++i)
			{
				_tiles[i] = tile;
			}
		};
		void Fill(const Map& map)
		{
			if (map._range != _range) throw std::runtime_error("_T& Map<_T>::Fill(const Map& map) different ranges");
			int len = GetLength();
			for (int i = 0; i < len; ++i)
			{
				_tiles[i] = map._tiles[i];
			}
		}
		template<typename _V>
		void Fill(const Map<_V>& map, std::function<_F(const _V&)> convert)
		{
			if (map._range != _range) throw std::runtime_error("_T& Map<_T>::Fill(const Map<_V>& map, std::function<_T(_V)> convert) different ranges");
			int len = GetLength();
			for (int i = 0; i < len; ++i)
			{
				_tiles[i] = convert(map._tiles[i]);
			}
		}
		void Fill(std::function<_F()> gen)
		{
			int len = GetLength();
			for (int i = 0; i < len; ++i)
			{
				_tiles[i] = gen();
			}
		};
		void FillIndexed(std::function<_F(const Point&)> gen)
		{
			Point::EnumerateRectangle(GetRange(), [&] (const Point& p)
			{
				GetElement(p) = gen(p);
			});
		};
	};
}