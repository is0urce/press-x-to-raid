// name: Graph.h
// type: c++ header file
// desc: opengl rendering
// auth: is0urce

// extends ogl with drawing functions

#pragma once

#include "vao.h"
#include "Point.h"

namespace px
{
	namespace mech
	{
		class Game;
	}
	namespace ui
	{
		class Gui;
	}
	namespace shell
	{
		class Perception;
		class Font;
		class Graph
		{
			// Attributes
		private:
			bool _init;
			// sizes
			double _camerafactor;
			double _cameramaxfactor;
			double _cameraminfactor;
			// updated & calculatet in Update virtual method
			int _width, _height;
			double _aspect;

			struct
			{
				VAO vao;
				GLuint program;
			} _background, _tiles, _units, _uitext, _uiback, _notify, _particle;

			std::unique_ptr<Font> _font;
			std::unique_ptr<Font> _guifont;
			GLuint _glyphtexture;
			GLuint _uitexture;

			// ctor & dtor
		public:
			Graph();
			virtual ~Graph();

			// virtuals
		protected:
			virtual void Swap() const = 0;
			virtual void UpdateGraphics(int &w, int &h) const = 0;

			// methods
		private:
			void Update();
			void GlyphTexture(bool force);
			void FillTiles(VAO &tiles, const Perception &perception);
			void FillUnits(VAO &units, const Perception &perception);
			void FillNotify(VAO &notify, const Perception &perception, Font &font);
			void FillUI(VAO &text, VAO &back, const ui::Gui &gui);

		protected:
			void Init();
			void Free();

		public:
			int GetWidth() const;
			int GetHeight() const;
			void Draw(const mech::Game &game, double phase);
			void Scale(int delta);
			Point WorldCoordinates(int mouse_x, int mouse_y) const;
			Point UICoordinates(int mouse_x, int mouse_y) const;
		};
	}
}