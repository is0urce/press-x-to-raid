// name: "ogl.h"
// type: c++ header
// desc: class declaration
// auth: is0urce

// base platform-specific opengl graphics handling
// You shold call Bind or constuct with HWND param

#pragma once

#include "Graph.h"

#include "gl\glew.h"
#include <gl\gl.h>
#include <gl\glu.h>
#include <wingdi.h>

namespace px
{
	namespace shell
	{
		class Ogl : public Graph
		{
		private:
			HWND _hWnd;
			HDC _hDC;
			HGLRC _hRC; // ogl rendering context
			GLuint _pixelformat;
			bool _binded;

		public:
			Ogl();
			Ogl(HWND hWnd);
			virtual ~Ogl();

		protected:
			virtual void UpdateGraphics(int &w, int &h) const;
			virtual void Swap() const;

		public:
			void Bind(HWND hWnd);
			void Release();
			bool IsBinded() const;
		};
	}
}