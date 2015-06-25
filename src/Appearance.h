// name: Appearance.h
// type: c++ header
// desc: container class for generic world objects appearance
// auth: is0urce

#pragma once

#include "Color.h"

namespace px
{
	struct Appearance
	{
	public:
		typedef int Image;
	private:
		Image _image;
		Color _color;
		double _size;

	public:
		Appearance(Image i, Color c, double s) : _image(i), _color(c), _size(s) {};
		Appearance(Image i, Color c) : _image(i), _color(c), _size(1.0) {};
		Appearance(Image i) : _image(i), _color(Color::White()), _size(1.0) {};
		Appearance() : _image('?'), _color(Color::White()), _size(1.0) {};

	public:
		Image GetImage() const { return _image; }
		const Color& GetColor() const { return _color; }
		void SetImage(Image img)
		{
			_image = img;
		}
		void SetColor(const Color& col)
		{
			_color = col;
		}
	};
}