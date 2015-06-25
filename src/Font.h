// name: "Font.h"
// type: c++
// desc: class header
// auth: is0urce

#pragma once

#include <ft2build.h>
#include FT_FREETYPE_H

namespace px
{
	namespace shell
	{
		struct Glyph
		{
			double top, bottom, left, right; // texture coordinates
			double width, height; // [0, 1] size
			double advance;
			int pixwidth, pixheight; // sizes in pixels

			// baseline offsets
			double vertical, horisontal;
			int pixvertical, pixhorisontal;

			Glyph() : width(0), height(0) {};
		};

		struct FontTexture
		{
			int width;
			int height;
			char* data;
		};

		class Font
		{
		private:
			static int _count;
			static FT_Library _lib;
			FT_Face _face;
			int _size;
			std::map<int, std::unique_ptr<Glyph>> _letters;
			std::unique_ptr<std::unique_ptr<Glyph>[]> _base;
			std::unique_ptr<char[]> _atlas;
			int _width, _height; // atlas size
			int _penx, _peny, _nextline;
			FontTexture _texture;
			bool _dirty; // is updated?

		public:
			Font(const char* name, int size);
			~Font();
		private:
			Font(const Font&); // disable copy

		public:
			void Include(int uplus);
			void Include(int uplus, int target_index);
			Glyph* GetGlyph(int uplus);
			FontTexture* GetTexture();
			bool IsUpdated() const;
			int GetSize() const;
			double GetKerning(int left, int right) const;

		private:
			std::unique_ptr<Glyph> Raster(int uplus);
		};
	}
}