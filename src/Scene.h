// name: Scene.h
// type: c++ header
// desc: 'Scene' class definition
// auth: is0urce

#pragma once

#include "Appearance.h"
#include "Tile.h"
#include "Map.h"

namespace px
{
	namespace vr
	{
		class Unit;

		class Scene
		{
			// types
		private:
			class PositionLess
			{
			public:
				bool operator() (const Point& a, const Point& b)
				{
					return std::tie(a.X, a.Y) < std::tie(b.X, b.Y);
				}
			};
		public:
			typedef std::shared_ptr<Unit> UnitPtr;
			typedef std::map<Point, UnitPtr, PositionLess> UnitList;

			// attributes
		private:
			// map
			Tile<Appearance> _default;
			std::unique_ptr<Map<Tile<Appearance>>> _map;

			// units
			UnitList _objects; // position sorted

			// ctor & dtor
		public:
			Scene();
			virtual ~Scene();
		private:
			Scene(const Scene&); // disable copy

			// Methods
		public:
			Tile<Appearance>& GetTile(const Point& point);
			const Tile<Appearance>& ReadTile(const Point& point) const;
			const UnitList& GetUnits() const;
			void Add(UnitPtr unit, const Point& position);
			void Move(UnitPtr unit, const Point& position);
			void Remove(UnitPtr unit);
			void Clear();
			int Count() const;
			void Tick(int delta);
			bool IsTransparent(const Point& point) const;
			bool IsTraversable(const Point& point) const;
			UnitPtr GetBlockingUnit(const Point& point) const;
		};
	}
}