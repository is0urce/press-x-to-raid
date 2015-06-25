// name: Tile.h
// type: c++ header
// desc: class declaration
// auth: is0urce

#pragma once

namespace px
{
	namespace vr
	{
		template <typename _A>
		class Tile
		{
			// attributes
		private:
			_A _appearance;
			bool _traversable;
			bool _transparent;

			// ctor & dtor
		public:
			Tile() {};
			~Tile() {};

			// methods
		public:
			_A& GetAppearance() { return _appearance; };
			bool IsTraversable() const { return _traversable; };
			bool IsTransparent() const { return _transparent; };

			void SetTraversable(bool ground) { _traversable = ground; };
			void SetTransparent(bool opaque) { _transparent = opaque; };
			void SetAppearance(const _A& appearance) { _appearance = appearance; };
			void MakeWall() { _traversable = _transparent = false; };
			void MakeGround() { _traversable = _transparent = true; };
		};
	}
}