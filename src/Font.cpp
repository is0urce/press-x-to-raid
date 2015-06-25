// name: "Font.cpp"
// type: c++
// desc: class implementation
// auth: is0urce

#include "stdafx.h"

#include "Font.h"

#include <string>

using namespace px::shell;

// CONSTANTS

namespace
{
	const int MULTIPLIER = 16; // atlas width in em
	const int FONT_FILL = 128; // precashe size
}

// IMPLEMENTATION

FT_Library Font::_lib;
int Font::_count = 0;
//std::mutex font_mutex; // no mutexes for now as crt debug memory leak report issue

Font::Font(const char* path, int size) : _size(size), _penx(0), _peny(0), _nextline(0), _dirty(true), _base(new std::unique_ptr<Glyph>[FONT_FILL])
{
	if (path == 0) throw std::runtime_error("Font::Font(const char* path, int size) path == 0");
	if (size <= 0) throw std::runtime_error("Font::Font(const char* path, int size) size <= 0");

	// init freetype if first
	//std::lock_guard<std::mutex> l(font_mutex);
	FT_Error error;
	if (_count == 0)
	{
		error = FT_Init_FreeType(&_lib);
		if (error)
		{
			throw std::runtime_error("FT_Init_FreeType");
		}
	}
	_count++;

	error = FT_New_Face(_lib, path, 0, &_face);
	if (error)
	{
		std::string message = std::string("FT_New_Face error path='") + path + "'";
		if (error == FT_Err_Unknown_File_Format)
		{
			throw std::runtime_error(message + " font format not supported");
		}
		else
		{
			throw std::runtime_error(message);
		}
	}
	error = FT_Set_Pixel_Sizes(_face, 0, _size);

	_width = 2;
	while (_width < _size * MULTIPLIER)
	{
		_width *= 2;
	}
	_height = _width;
	_atlas.reset(new char[_width * _height]);
	memset(_atlas.get(), 0, sizeof(_atlas[0]) * _width * _height);

	_texture.width = _width;
	_texture.height = _height;
	_texture.data = _atlas.get();

	// include base
	for (int i = 0; i < FONT_FILL; i++)
	{
		_base[i] = Raster(i);
	}
}

Font::~Font()
{
	FT_Done_Face(_face);

	//std::lock_guard<std::mutex> l(font_mutex);
	_count--;
	if (_count == 0)
	{
		FT_Done_FreeType(_lib);
	}
}

void Font::Include(int uplus)
{
	Include(uplus, uplus);
}

std::unique_ptr<Glyph> Font::Raster(int uplus)
{
	FT_Error error;
	int index = FT_Get_Char_Index(_face, uplus);
	error = FT_Load_Glyph(_face, index, FT_LOAD_DEFAULT);
	if (error) throw std::runtime_error("FT_Load_Glyph");
	error = FT_Render_Glyph(_face->glyph, FT_RENDER_MODE_NORMAL);
	if (error) throw std::runtime_error("FT_Render_Glyph");
	int w = _face->glyph->bitmap.width;
	int h = _face->glyph->bitmap.rows;

	int xstride = w + w / 4;
	int ystride = h + h / 4;
	if (_penx + xstride > _width)
	{
		_penx = 0;
		_peny += _nextline;
		_nextline = 0;
	}
	if (_peny + ystride > _height)
	{
		throw std::runtime_error("atlas too small");
	}

	std::unique_ptr<Glyph> glyph(new Glyph());
	glyph->advance = (_face->glyph->advance.x >> 6) / double(_size);
	glyph->top = double(_peny) / _height;
	glyph->bottom = double(_peny + h) / _height;
	glyph->left = double(_penx) / _width;
	glyph->right = double(_penx + w) / _width;
	glyph->pixwidth = w;
	glyph->pixheight = h;
	glyph->pixvertical = _face->glyph->bitmap_left;
	glyph->pixhorisontal = _face->glyph->bitmap_top;
	glyph->width = double(w) / _size;
	glyph->height = double(h) / _size;
	glyph->vertical = (_face->glyph->bitmap_left) / double(_size);
	glyph->horisontal = (_face->glyph->bitmap_top) / double(_size);

	// copy bitmap
	for (int i = 0; i < w; i++)
	{
		for (int j = 0; j < h; j++)
		{
			_atlas[(_peny + j) * _width + _penx + i] = _face->glyph->bitmap.buffer[i + j * w];
		}
	}
	_penx += xstride;
	_nextline = std::max<int>(_nextline, ystride);

	_dirty = true;
	return glyph;
}

void Font::Include(int uplus, int into)
{
	_letters[into] = Raster(uplus);
}

Glyph* Font::GetGlyph(int uplus)
{
	if (uplus < 0)
	{
		std::stringstream ss;
		ss << "Font::GetGlyph(int uplus) uplus < 0, uplus=" << uplus;
		throw std::runtime_error(ss.str());
	}
	if (uplus < FONT_FILL) return _base[uplus].get();

	std::unique_ptr<Glyph>& glyph = _letters[uplus];
	if (glyph == 0)
	{
		glyph = Raster(uplus);
	}
	return glyph.get();
}

FontTexture* Font::GetTexture()
{
	_dirty = false;
	return &_texture;
}

bool Font::IsUpdated() const
{
	return _dirty;
}

int Font::GetSize() const
{
	return _size;
}

double Font::GetKerning(int l1, int l2) const
{
	FT_Vector kerning;
	FT_Get_Kerning(_face, l1, l2, FT_KERNING_DEFAULT, &kerning);
	return (kerning.x >> 6) / double(_size);
	//return 1;
}