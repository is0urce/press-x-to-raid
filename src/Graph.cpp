// name: Graph.cpp
// type: c++ class implementation
// desc: opengl rendering
// auth: is0urce

#include "stdafx.h"

#include "Graph.h"

#include "gl\glew.h"

#include "glsl.h"

#include "Perception.h"
#include "Game.h"
#include "Font.h"
#include "Vector.h"

#include "APanel.h"
#include "Gui.h"

using namespace px;
using namespace px::shell;

// HELPERS AND CONSTANTS

namespace
{
	const int quad_points = 4;
	const char* font_path_main = "CODE2000.TTF";
	//const char* font_path_main = "FiraMono-Bold.ttf";
	const char* font_path_gui = "everson-mono-unicode.ttf";
	//const char* font_path_gui = "FiraMono-Bold.ttf";
	const int font_size_main = 32;
	const int font_size_gui = 20;

	const int ui_cell_width = 16;
	const int ui_cell_height = 16;

	// array size (specific)
	template <typename T>
	size_t size_of_array(const T& a)
	{
		return (sizeof(a) / sizeof(a[0]));
	}

	inline Vector precise_shift(const Perception &perception)
	{
		return Vector(perception.GetWidth() * -0.5, perception.GetHeight() * -0.50); // align to center
	}
}

// CLASS IMPLEMENTATION CTOR DTOR

Graph::Graph() :
	_init(false),
	_camerafactor(0.05), _cameramaxfactor(0.1), _cameraminfactor(0.01),
	_font(new Font(font_path_main, font_size_main)),
	_guifont(new Font(font_path_gui, font_size_gui))
{
}

Graph::~Graph()
{
	if (_init)
	{
		Free();
	}
}

// METHODS

void Graph::Scale(int delta)
{
	_camerafactor *= 1.0 + (delta / 1000.0);
	_camerafactor = std::min<double>(_camerafactor, _cameramaxfactor);
	_camerafactor = std::max<double>(_camerafactor, _cameraminfactor);
}

void Graph::Update()
{
	UpdateGraphics(_width, _height);
	_aspect = float(_width) / float(_height);
}

void Graph::GlyphTexture(bool force)
{
	if (force || _font->IsUpdated())
	{
		FontTexture* texture = _font->GetTexture();

		glBindTexture(GL_TEXTURE_2D, _glyphtexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 8);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texture->width, texture->height, 0, GL_RED, GL_UNSIGNED_BYTE, texture->data);
		glEnable(GL_TEXTURE_2D);
		glGenerateMipmap(GL_TEXTURE_2D);  // generate mipmaps here.
		// mipmap filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		//glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST); // filtering, sharp switching between mipmaps
	}
	if (force || _guifont->IsUpdated() )
	{
		FontTexture* texture = _guifont->GetTexture();

		glBindTexture(GL_TEXTURE_2D, _uitexture);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0); // no mipmaps
		glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, texture->width, texture->height, 0, GL_RED, GL_UNSIGNED_BYTE, texture->data);
		// mipmap filters
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

void Graph::Init()
{
	if (_init)
	{
		Free();
	}
	_init = true;

	glGenTextures(1, &_glyphtexture);
	glGenTextures(1, &_uitexture);

	int back[] = {2, 4};
	_background.vao.Init(size_of_array(back), back);
	_background.program = program("shaders/ground");

	int tiles[] = {2, 2, 4, 4};
	_tiles.vao.Init(size_of_array(tiles), tiles);
	_tiles.program = program("shaders/tiles");

	int units[] = {2, 2, 4, 2};
	_units.vao.Init(size_of_array(units), units);
	_units.program = program("shaders/units");

	int uitext[] = {2, 2, 4};
	_uitext.vao.Init(size_of_array(uitext), uitext);
	_uitext.program = program("shaders/uitext");

	int uiback[] = {2, 4};
	_uiback.vao.Init(size_of_array(uiback), uiback);
	_uiback.program = program("shaders/uisolid");

	int notify[] = {2, 2, 4};
	_notify.vao.Init(size_of_array(notify), notify);
	_notify.program = program("shaders/notify");
}

void Graph::Free()
{
	if (_init)
	{
		glDeleteTextures(1, &_glyphtexture);
		glDeleteTextures(1, &_uitexture);

		_init = false;
	}
}

inline void FillBackground(VAO& back, const Perception& perception)
{
	int w = perception.GetWidth();
	int h = perception.GetHeight();
	int size = w * h;
	const int vertexdepth = 2;
	const int colordepth = 4;

	std::unique_ptr<GLfloat[]> vertice(new GLfloat[size * quad_points * vertexdepth]);
	std::unique_ptr<GLfloat[]> color(new GLfloat[size * quad_points * colordepth]);
	std::unique_ptr<GLuint[]> indices(new GLuint[size * 6]);

	// vertex attributes
	int vertexoffset = 0;
	int coloroffset = 0;
	Point::EnumerateRectangle(Point(0, 0), Point(w, h), [&] (Point position)
	{
		position.Moved(Point(0, 0)).Write(&vertice[vertexoffset + 0 * vertexdepth]);
		position.Moved(Point(0, 1)).Write(&vertice[vertexoffset + 1 * vertexdepth]);
		position.Moved(Point(1, 1)).Write(&vertice[vertexoffset + 2 * vertexdepth]);
		position.Moved(Point(1, 0)).Write(&vertice[vertexoffset + 3 * vertexdepth]);
		for (int i = 0; i < quad_points; i++)
		{
			perception.GetAppearance(position).GetColor().Write(&color[coloroffset + i * colordepth]);
		}

		vertexoffset += vertexdepth * quad_points;
		coloroffset += colordepth * quad_points;
	});

	// indices
	int indexoffset = 0;
	for (int i = 0; i < size; ++i)
	{
		indices[indexoffset + 0] = indices[indexoffset + 3] = i * quad_points + 0;
		indices[indexoffset + 1] = indices[indexoffset + 5] = i * quad_points + 2;
		indices[indexoffset + 2] = i * quad_points + 1;
		indices[indexoffset + 4] = i * quad_points + 3;
		indexoffset += 6; // indexes in quad = 2 triangle
	}

	// bind
	GLfloat* attributes[] = {vertice.get(), color.get()};
	back.Fill(size * quad_points, size * 6, attributes, indices.get());
}

void Graph::FillTiles(VAO& tiles, const Perception &perception)
{
	int w = perception.GetWidth();
	int h = perception.GetHeight();
	int size = w * h;
	Vector precise(- w / 2.0, - h / 2.0); // align to center
	const int vertexdepth = 2;
	const int texturedepth = 2;
	const int colordepth = 4;

	std::unique_ptr<GLfloat[]> vertice(new GLfloat[size * quad_points * vertexdepth]);
	std::unique_ptr<GLfloat[]> texture(new GLfloat[size * quad_points * texturedepth]);
	std::unique_ptr<GLfloat[]> color(new GLfloat[size * quad_points * colordepth]);
	std::unique_ptr<GLfloat[]> colorprev(new GLfloat[size * quad_points * colordepth]);
	std::unique_ptr<GLuint[]> indices(new GLuint[size * 6]);

	// vertex attributes
	int vertexoffset = 0;
	int coloroffset = 0;
	int textureoffset = 0;
	Point::EnumerateRectangle(Point(w, h), [&] (const Point& position)
	{
		Glyph* glyph = _font->GetGlyph(perception.GetAppearance(position).GetImage());
		double width = glyph->width / 2;
		double height = glyph->height / 2;
		(position + precise + Vector(0.5 - width, 0.5 - height)).Write(&vertice[vertexoffset + 0 * vertexdepth]);
		(position + precise + Vector(0.5 - width, 0.5 + height)).Write(&vertice[vertexoffset + 1 * vertexdepth]);
		(position + precise + Vector(0.5 + width, 0.5 + height)).Write(&vertice[vertexoffset + 2 * vertexdepth]);
		(position + precise + Vector(0.5 + width, 0.5 - height)).Write(&vertice[vertexoffset + 3 * vertexdepth]);
		for (int i = 0; i < quad_points; i++)
		{
			(perception.GetAppearance(position).GetColor() * perception.GetLight(position)).Write(&color[coloroffset + i * colordepth]);
			(perception.GetAppearance(position).GetColor() * perception.GetPreviousLight(position)).Write(&colorprev[coloroffset + i * colordepth]);
		}
		texture[textureoffset + 0] = texture[textureoffset + 2] = (GLfloat)glyph->left;
		texture[textureoffset + 1] = texture[textureoffset + 7] = (GLfloat)glyph->bottom;
		texture[textureoffset + 4] = texture[textureoffset + 6] = (GLfloat)glyph->right;
		texture[textureoffset + 3] = texture[textureoffset + 5] = (GLfloat)glyph->top;

		vertexoffset += vertexdepth * quad_points;
		textureoffset += texturedepth * quad_points;
		coloroffset += colordepth * quad_points;
	});

	// indices
	int indexoffset = 0;
	for (int i = 0; i < size; ++i)
	{
		indices[indexoffset + 0] = indices[indexoffset + 3] = i * quad_points + 0;
		indices[indexoffset + 1] = indices[indexoffset + 5] = i * quad_points + 2;
		indices[indexoffset + 2] = i * quad_points + 1;
		indices[indexoffset + 4] = i * quad_points + 3;
		indexoffset += 6; // indexes in quad = 2 triangle
	}

	// bind
	GLfloat* attributes[] = {vertice.get(), texture.get(), color.get(), colorprev.get()};
	tiles.Fill(size * quad_points, size * 6, attributes, indices.get());
}

void Graph::FillUnits(VAO &units, const Perception &perception)
{
	int w = perception.GetWidth();
	int h = perception.GetHeight();
	Vector precise(- w / 2.0, - h / 2.0); // align to center
	const Perception::UnitList& list = perception.GetUnits();
	int size = list.size();

	const int vertexdepth = 2;
	const int texturedepth = 2;
	const int colordepth = 4;

	std::unique_ptr<GLfloat[]> vertice(new GLfloat[size * quad_points * vertexdepth]);
	std::unique_ptr<GLfloat[]> texture(new GLfloat[size * quad_points * texturedepth]);
	std::unique_ptr<GLfloat[]> color(new GLfloat[size * quad_points * colordepth]);
	std::unique_ptr<GLfloat[]> positionprev(new GLfloat[size * quad_points * vertexdepth]);
	std::unique_ptr<GLuint[]> indices(new GLuint[size * 6]);

	// vertex attributes
	int vertexoffset = 0;
	int textureoffset = 0;
	int coloroffset = 0;

	for (auto unit = list.begin(); unit != list.end(); ++unit)
	{
		Glyph* glyph = _font->GetGlyph(unit->GetAppearance().GetImage());
		const Point& position(unit->GetPosition());
		double width = glyph->width / 2;
		double height = glyph->height / 2;
		(position + precise + Vector(0.5 - width, 0.5 - height)).Write(&vertice[vertexoffset + 0 * vertexdepth]);
		(position + precise + Vector(0.5 - width, 0.5 + height)).Write(&vertice[vertexoffset + 1 * vertexdepth]);
		(position + precise + Vector(0.5 + width, 0.5 + height)).Write(&vertice[vertexoffset + 2 * vertexdepth]);
		(position + precise + Vector(0.5 + width, 0.5 - height)).Write(&vertice[vertexoffset + 3 * vertexdepth]);

		const Point& prev(unit->GetPreviousPosition());
		(prev + precise + Vector(0.5 - width, 0.5 - height)).Write(&positionprev[vertexoffset + 0 * vertexdepth]);
		(prev + precise + Vector(0.5 - width, 0.5 + height)).Write(&positionprev[vertexoffset + 1 * vertexdepth]);
		(prev + precise + Vector(0.5 + width, 0.5 + height)).Write(&positionprev[vertexoffset + 2 * vertexdepth]);
		(prev + precise + Vector(0.5 + width, 0.5 - height)).Write(&positionprev[vertexoffset + 3 * vertexdepth]);

		for (int i = 0; i < quad_points; i++)
		{
			unit->GetAppearance().GetColor().Write(&color[coloroffset + i * colordepth]);
		}
		texture[textureoffset + 0] = texture[textureoffset + 2] = (GLfloat)glyph->left;
		texture[textureoffset + 1] = texture[textureoffset + 7] = (GLfloat)glyph->bottom;
		texture[textureoffset + 4] = texture[textureoffset + 6] = (GLfloat)glyph->right;
		texture[textureoffset + 3] = texture[textureoffset + 5] = (GLfloat)glyph->top;

		vertexoffset += vertexdepth * quad_points;
		textureoffset += texturedepth * quad_points;
		coloroffset += colordepth * quad_points;
	}

	// indices
	int indexoffset = 0;
	for (int i = 0; i < size; ++i)
	{
		indices[indexoffset + 0] = indices[indexoffset + 3] = i * quad_points + 0;
		indices[indexoffset + 1] = indices[indexoffset + 5] = i * quad_points + 2;
		indices[indexoffset + 2] = i * quad_points + 1;
		indices[indexoffset + 4] = i * quad_points + 3;
		indexoffset += 6; // indexes in quad = 2 triangle
	}

	// bind
	GLfloat* attributes[] = {vertice.get(), texture.get(), color.get(), positionprev.get()};
	units.Fill(size * quad_points, size * 6, attributes, indices.get());
}

void Graph::FillNotify(VAO &notify, const Perception &perception, Font &font)
{
	Vector precise = precise_shift(perception);
	double size_mult = 0.5;
	const Perception::NotifyList messages = perception.GetNotifications();

	int size = 0;
	for (auto i = messages.begin(); i != messages.end(); ++i)
	{
		size += i->GetText().length();
	}

	const int vertexdepth = 2;
	const int texturedepth = 2;
	const int colordepth = 4;

	// vertex attributes
	int vertexoffset = 0;
	int textureoffset = 0;
	int coloroffset = 0;

	std::unique_ptr<GLfloat[]> vertice(new GLfloat[size * quad_points * vertexdepth]);
	std::unique_ptr<GLfloat[]> texture(new GLfloat[size * quad_points * texturedepth]);
	std::unique_ptr<GLfloat[]> color(new GLfloat[size * quad_points * colordepth]);
	std::unique_ptr<GLuint[]> indices(new GLuint[size * 6]);

	for (auto i = messages.begin(); i != messages.end(); ++i)
	{
		Vector pen = i->GetPosition();
		Perception::NotifyString string = i->GetText();
		int length = string.length();

		int prev_uplus;
		
		prev_uplus = 0;
		double total_width = 0;
		for (int k = 0; k < length; k++)
		{
			int uplus = int(string[k]);
			Glyph* glyph = font.GetGlyph(uplus);
			total_width += font.GetKerning(prev_uplus, uplus);
			total_width += glyph->advance;
			prev_uplus = uplus;
		}

		pen.Move(Vector(0.5 - total_width * 0.5 * size_mult, 1));

		prev_uplus = 0;
		for (int k = 0; k < length; k++)
		{
			int uplus = int(string[k]);
			Glyph* glyph = font.GetGlyph(uplus); // messages uses in-world font

			pen.X += font.GetKerning(prev_uplus, uplus) * size_mult;
			prev_uplus = uplus;

			double gw = glyph->width;
			double gh = glyph->height;
			double ghor = glyph->horisontal;

			gw *= size_mult;
			gh *= size_mult;
			ghor *= size_mult;

			(pen + precise + Vector( 0, ghor - gh)).Write(&vertice[vertexoffset + 0 * vertexdepth]);
			(pen + precise + Vector( 0, ghor     )).Write(&vertice[vertexoffset + 1 * vertexdepth]);
			(pen + precise + Vector(gw, ghor     )).Write(&vertice[vertexoffset + 2 * vertexdepth]);
			(pen + precise + Vector(gw, ghor - gh)).Write(&vertice[vertexoffset + 3 * vertexdepth]);

			pen.X += glyph->advance * size_mult;

			for (int p = 0; p < quad_points; p++) i->GetColor().Write(&color[coloroffset + colordepth * p]);

			// texture
			texture[textureoffset + 0] = texture[textureoffset + 2] = (GLfloat)glyph->left;
			texture[textureoffset + 1] = texture[textureoffset + 7] = (GLfloat)glyph->bottom;
			texture[textureoffset + 4] = texture[textureoffset + 6] = (GLfloat)glyph->right;
			texture[textureoffset + 3] = texture[textureoffset + 5] = (GLfloat)glyph->top;

			// next
			vertexoffset += vertexdepth * quad_points;
			textureoffset += texturedepth * quad_points;
			coloroffset += colordepth * quad_points;
		}
	}

	// indices
	int indexoffset = 0;
	for (int i = 0; i < size; ++i)
	{
		indices[indexoffset + 0] = indices[indexoffset + 3] = i * quad_points + 0;
		indices[indexoffset + 1] = indices[indexoffset + 5] = i * quad_points + 2;
		indices[indexoffset + 2] = i * quad_points + 1;
		indices[indexoffset + 4] = i * quad_points + 3;
		indexoffset += 6; // indexes in quad = 2 triangle
	}

	// bind
	GLfloat* attributes[] = {vertice.get(), texture.get(), color.get()};
	notify.Fill(size * quad_points, size * 6, attributes, indices.get());
}

void Graph::FillUI(VAO &text, VAO& back, const ui::Gui &gui)
{
	Point range = gui.GetRange();
	int w = range.X;
	int h = range.Y;
	int size = w * h;

	const int vertexdepth = 2;
	const int texturedepth = 2;
	const int colordepth = 4;

	std::unique_ptr<GLfloat[]> vertex_front(new GLfloat[size * quad_points * vertexdepth]); // vertex
	std::unique_ptr<GLfloat[]> vertex_back(new GLfloat[size * quad_points * vertexdepth]); // vertex
	std::unique_ptr<GLfloat[]> texture(new GLfloat[size * quad_points * texturedepth]);
	std::unique_ptr<GLfloat[]> color_front(new GLfloat[size * quad_points * colordepth]); // color
	std::unique_ptr<GLfloat[]> color_back(new GLfloat[size * quad_points * colordepth]); // color
	std::unique_ptr<GLuint[]> indices(new GLuint[size * 6]);

	// vertex attributes
	int vertexoffset = 0;
	int textureoffset = 0;
	int coloroffset = 0;

	range.EnumerateRange([&] (Point position)
	{
		ui::Symbol &s = gui.GetElement(position);
		Glyph* glyph = _guifont->GetGlyph(s.plus);

		position = Point(position.X, h - position.Y - 1);

		position.Moved(Point(0, 0)).Multiplied(ui_cell_width, ui_cell_height).Write(&vertex_back[vertexoffset + 0 * vertexdepth]);
		position.Moved(Point(0, 1)).Multiplied(ui_cell_width, ui_cell_height).Write(&vertex_back[vertexoffset + 1 * vertexdepth]);
		position.Moved(Point(1, 1)).Multiplied(ui_cell_width, ui_cell_height).Write(&vertex_back[vertexoffset + 2 * vertexdepth]);
		position.Moved(Point(1, 0)).Multiplied(ui_cell_width, ui_cell_height).Write(&vertex_back[vertexoffset + 3 * vertexdepth]);

		int vx = ui_cell_width / 2 - glyph->pixwidth / 2;
		int vxd = vx + glyph->pixwidth;
		int vyd = glyph->pixhorisontal + 1;
		int vy = vyd - glyph->pixheight;

		position.Multiplied(ui_cell_width, ui_cell_height).Moved(Point( vx,  vy)).Write(&vertex_front[vertexoffset + 0 * vertexdepth]);
		position.Multiplied(ui_cell_width, ui_cell_height).Moved(Point( vx, vyd)).Write(&vertex_front[vertexoffset + 1 * vertexdepth]);
		position.Multiplied(ui_cell_width, ui_cell_height).Moved(Point(vxd, vyd)).Write(&vertex_front[vertexoffset + 2 * vertexdepth]);
		position.Multiplied(ui_cell_width, ui_cell_height).Moved(Point(vxd,  vy)).Write(&vertex_front[vertexoffset + 3 * vertexdepth]);
		//position.Moved(Point(0, 0)).Multiplied(ui_cell_width, ui_cell_height).Write(&vertex_front[vertexoffset + 0 * vertexdepth]);
		//position.Moved(Point(0, 1)).Multiplied(ui_cell_width, ui_cell_height).Write(&vertex_front[vertexoffset + 1 * vertexdepth]);
		//position.Moved(Point(1, 1)).Multiplied(ui_cell_width, ui_cell_height).Write(&vertex_front[vertexoffset + 2 * vertexdepth]);
		//position.Moved(Point(1, 0)).Multiplied(ui_cell_width, ui_cell_height).Write(&vertex_front[vertexoffset + 3 * vertexdepth]);

		for (int i = 0; i < quad_points; ++i)
		{
			s.color.Write(&color_front[coloroffset + i * colordepth]);
			s.back.Write(&color_back[coloroffset + i * colordepth]);
		}
		texture[textureoffset + 0] = texture[textureoffset + 2] = (GLfloat)glyph->left;
		texture[textureoffset + 1] = texture[textureoffset + 7] = (GLfloat)glyph->bottom;
		texture[textureoffset + 4] = texture[textureoffset + 6] = (GLfloat)glyph->right;
		texture[textureoffset + 3] = texture[textureoffset + 5] = (GLfloat)glyph->top;

		vertexoffset += vertexdepth * quad_points;
		textureoffset += texturedepth * quad_points;
		coloroffset += colordepth * quad_points;
	});

	// indices
	int indexoffset = 0;
	for (int i = 0; i < size; ++i)
	{
		indices[indexoffset + 0] = indices[indexoffset + 3] = i * quad_points + 0;
		indices[indexoffset + 1] = indices[indexoffset + 5] = i * quad_points + 2;
		indices[indexoffset + 2] = i * quad_points + 1;
		indices[indexoffset + 4] = i * quad_points + 3;
		indexoffset += 6; // indexes in quad = 2 triangle
	}

	// bind
	GLfloat* front_attributes[] = {vertex_front.get(), texture.get(), color_front.get()};
	GLfloat* back_attributes[] = {vertex_back.get(), color_back.get()};
	text.Fill(size * quad_points, size * 6, front_attributes, indices.get());
	back.Fill(size * quad_points, size * 6, back_attributes, indices.get());
}

void Graph::Draw(const mech::Game &game, double w)
{
	Update();
	if (_width <= 0 || _height <= 0) return;

	const Perception &perception = game.GetPerception();
	GLfloat aspect = (GLfloat) _aspect;
	GLfloat scale = (GLfloat) _camerafactor;
	GLfloat move_phase = (GLfloat) w * 5;
	GLfloat notify_phase = (GLfloat) w * 0.5f;

	glViewport(0, 0, (GLsizei) _width, (GLsizei) _height);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	//FillBackground(_background.vao, perception);
	FillTiles(_tiles.vao, perception);
	FillUnits(_units.vao, perception);
	FillNotify(_notify.vao, perception, *_font);
	GlyphTexture(false);

	//glUseProgram(_background.program);
	//glUniform1f(glGetUniformLocation(_background.program, "aspect"), aspect);
	//glUniform1f(glGetUniformLocation(_background.program, "scale"), scale);
	//_background.vao.Draw();

	Point move = perception.GetMovement();
	GLfloat movex = (GLfloat) move.X;
	GLfloat movey = (GLfloat) move.Y;

	glUseProgram(_tiles.program);
	glUniform1f(glGetUniformLocation(_tiles.program, "aspect"), aspect);
	glUniform1f(glGetUniformLocation(_tiles.program, "scale"), scale);
	glUniform2f(glGetUniformLocation(_tiles.program, "movement"), movex, movey);
	glUniform1f(glGetUniformLocation(_tiles.program, "phase"), move_phase);
	glUniform1i(glGetUniformLocation(_tiles.program, "glyphTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, _glyphtexture);
	_tiles.vao.Draw();

	glUseProgram(_units.program);
	glUniform1f(glGetUniformLocation(_units.program, "aspect"), aspect);
	glUniform1f(glGetUniformLocation(_units.program, "scale"), scale);
	glUniform2f(glGetUniformLocation(_units.program, "movement"), movex, movey);
	glUniform1f(glGetUniformLocation(_units.program, "phase"), move_phase);
	glUniform1i(glGetUniformLocation(_units.program, "glyphTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, _glyphtexture);
	_units.vao.Draw();

	glUseProgram(_notify.program);
	glUniform1f(glGetUniformLocation(_notify.program, "aspect"), aspect);
	glUniform1f(glGetUniformLocation(_notify.program, "scale"), scale);
	glUniform2f(glGetUniformLocation(_notify.program, "movement"), movex, movey);
	glUniform1f(glGetUniformLocation(_notify.program, "phase"), move_phase);
	glUniform1f(glGetUniformLocation(_notify.program, "timer"), notify_phase);
	/*glUniform1f(glGetUniformLocation(_notify.program, "outline"), (GLfloat) (0.25f / _font->GetTexture()->width));*/
	//glUniform1f(glGetUniformLocation(_notify.program, "outline"), 0);
	glUniform1i(glGetUniformLocation(_notify.program, "glyphTexture"), 0);
	glBindTexture(GL_TEXTURE_2D, _glyphtexture);
	_notify.vao.Draw();

	// ui
	int guiw = _width / ui_cell_width;
	int guih = _height / ui_cell_height;
	if (guiw > 0 && guih > 0)
	{
		game.GetInterface()->Resize(guiw, guih);
		game.GetInterface()->Draw();
		FillUI(_uitext.vao, _uiback.vao, *game.GetInterface()->GetTable());
		GlyphTexture(false);
		glUseProgram(_uiback.program);
		glUniform2f(glGetUniformLocation(_uiback.program, "scale"), (GLfloat)_width, (GLfloat)_height);
		glUniform2f(glGetUniformLocation(_uiback.program, "offsets"), (GLfloat)(_width % ui_cell_width / 2), (GLfloat)(_height % ui_cell_height / 2));
		glBindTexture(GL_TEXTURE_2D, _uitexture);
		_uiback.vao.Draw();

		glUseProgram(_uitext.program);
		glUniform2f(glGetUniformLocation(_uitext.program, "scale"), (GLfloat)_width, (GLfloat)_height);
		glUniform2f(glGetUniformLocation(_uitext.program, "offsets"), (GLfloat)(_width % ui_cell_width / 2), (GLfloat)(_height % ui_cell_height / 2));
		_uitext.vao.Draw();
	}

	Swap();
}

int Graph::GetWidth() const
{
	return _width;
}

int Graph::GetHeight() const
{
	return _height;
}

Point Graph::WorldCoordinates(int x, int y) const
{
	double fx =  2.0 * x / _width  - 1.0;
	double fy = -2.0 * y / _height + 1.0;
	fx = std::floor(fx / _camerafactor);
	fy = std::floor(fy / _camerafactor / _aspect);
	return Point((int)fx, (int)fy);
}

Point Graph::UICoordinates(int mouse_x, int mouse_y) const
{
	return Point((mouse_x - _width % ui_cell_width / 2) / ui_cell_width, (mouse_y - _height % ui_cell_height / 2) / ui_cell_height);
}