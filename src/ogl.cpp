// name: ogl.cpp
// type: c++
// desc: class implementation
// auth: is0urce

#include "stdafx.h"

#include "ogl.h"

#pragma comment(lib, "glew32.lib")
#pragma comment(lib, "opengl32.lib")

using namespace px::shell;

static PIXELFORMATDESCRIPTOR pfd =                  // pfd Tells Windows How We Want Things To Be
{
	sizeof(PIXELFORMATDESCRIPTOR),                  // Size Of This Pixel Format Descriptor
	1,                              // Version Number
	PFD_DRAW_TO_WINDOW |                        // Format Must Support Window
	PFD_SUPPORT_OPENGL |                        // Format Must Support OpenGL
	PFD_DOUBLEBUFFER,                       // Must Support Double Buffering
	PFD_TYPE_RGBA,                          // Request An RGBA Format
	8,                               // Select Our Color Depth
	0, 0, 0, 0, 0, 0,                       // Color Bits Ignored
	0,                              // No Alpha Buffer
	0,                              // Shift Bit Ignored
	0,                              // No Accumulation Buffer
	0, 0, 0, 0,                         // Accumulation Bits Ignored
	16,                             // 16Bit Z-Buffer (Depth Buffer)
	0,                              // No Stencil Buffer
	0,                              // No Auxiliary Buffer
	PFD_MAIN_PLANE,                         // Main Drawing Layer
	0,                              // Reserved
	0, 0, 0                             // Layer Masks Ignored
};

Ogl::Ogl() : _hWnd(0), _hRC(0), _binded(false)
{
}

Ogl::Ogl(HWND hWnd) : _hWnd(0), _hRC(0), _binded(false)
{
	Bind(hWnd);
}

Ogl::~Ogl()
{
	Release();
}

void Ogl::Bind(HWND hWnd)
{
	Release();

	_hWnd = hWnd;
	if (!(_hDC = GetDC(_hWnd)))
	{
		MessageBox(NULL, L"GetDC failed", L"ERROR", MB_OK|MB_ICONEXCLAMATION);
	}
	if (!(_pixelformat = ChoosePixelFormat(_hDC, &pfd)))             // Did Windows Find A Matching Pixel Format?
	{
		MessageBox(NULL, L"ChoosePixelFormat failed", L"ERROR", MB_OK|MB_ICONEXCLAMATION);
	}
	if (!SetPixelFormat(_hDC, _pixelformat, &pfd))               // Are We Able To Set The Pixel Format?
	{
		MessageBox(NULL, L"SetPixelFormat failed", L"ERROR", MB_OK|MB_ICONEXCLAMATION);
	}
	if (!(_hRC = wglCreateContext(_hDC)))
	{
		MessageBox(NULL, L"wglCreateContext - can't create a GL Rendering Context", L"ERROR", MB_OK|MB_ICONEXCLAMATION);
	}
	if (!wglMakeCurrent(_hDC, _hRC))                        // Try To Activate The Rendering Context
	{
		MessageBox(NULL, L"wglMakeCurrent - can't activate the GL Rendering Context", L"ERROR", MB_OK|MB_ICONEXCLAMATION);
	}

	glewInit();
	_binded = true;
	Init();
}

void Ogl::Release()
{
	if (_hRC)                                // Do We Have A Rendering Context?
	{
		if (!wglMakeCurrent(NULL,NULL))                 // Are We Able To Release The DC And RC Contexts?
		{
			MessageBox(NULL,L"Release Of DC And RC Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		if (!wglDeleteContext(_hRC))                 // Are We Able To Delete The RC?
		{
			MessageBox(NULL, L"Release Rendering Context Failed.", L"SHUTDOWN ERROR", MB_OK | MB_ICONINFORMATION);
		}
		_hRC = 0;
		if (_hDC)
		{
			ReleaseDC(_hWnd, _hDC);
			_hDC = 0;
		}
		Free();
	}
	_binded = false;
}

void Ogl::UpdateGraphics(int& w, int& h) const
{
	RECT rect;
	GetClientRect(_hWnd, &rect);
	w = rect.right - rect.left;
	h = rect.bottom - rect.top;
}

void Ogl::Swap() const
{
	SwapBuffers(_hDC);
}

bool Ogl::IsBinded() const
{
	return _binded;
}