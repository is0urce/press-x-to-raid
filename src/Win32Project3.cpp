// Win32Project3.cpp: определяет точку входа для приложения.
//

#include "stdafx.h"
#include "Win32Project3.h"

#define MAX_LOADSTRING 100

// Глобальные переменные:
HWND hWnd;
HINSTANCE hInst;								// текущий экземпляр
TCHAR szTitle[MAX_LOADSTRING];					// Текст строки заголовка
TCHAR szWindowClass[MAX_LOADSTRING];			// имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM				MyRegisterClass(HINSTANCE hInstance);
BOOL				InitInstance(HINSTANCE, int);
LRESULT CALLBACK	WndProc(HWND, UINT, WPARAM, LPARAM);

#include "ogl.h"
#include "Game.h"
#include "APanel.h"
#include "Timer.h"

using namespace px;

std::unique_ptr<shell::Ogl> gl;
std::unique_ptr<mech::Game> game;
ui::APanel *menu;

int APIENTRY _tWinMain(_In_ HINSTANCE hInstance,
					   _In_opt_ HINSTANCE hPrevInstance,
					   _In_ LPTSTR    lpCmdLine,
					   _In_ int       nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: разместите код здесь.
	MSG msg;
	HACCEL hAccelTable;

	// Инициализация глобальных строк
	LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInstance, IDC_WIN32PROJECT3, szWindowClass, MAX_LOADSTRING);

	MyRegisterClass(hInstance);

	// Выполнить инициализацию приложения:
	if (!InitInstance (hInstance, nCmdShow))
	{
		return FALSE;
	}
	hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WIN32PROJECT3));

	try
	{
		// create
		gl.reset(new shell::Ogl());
		game.reset(new mech::Game());

		// init
		gl->Bind(hWnd);
		menu = game->GetInterface();

		// timer & rng
		Timer timer;
		std::srand((unsigned int)timer.GetCounter());
		timer.Start();

		// main & message loop:
		auto last_turn = game->GetTurn();
		bool run = true;
		while (!game->IsFinished() && run)
		{
			auto current_turn = game->GetTurn();
			if (last_turn != current_turn)
			{
				timer.Start();
				last_turn = current_turn;
			}
			gl->Draw(*game, timer.GetCounter());

			// windows dispatcher
			while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE) != 0)
			{
				run &= (GetMessage(&msg, NULL, 0, 0) == TRUE);
				if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}
	}
	catch (std::runtime_error &exc)
	{
		wchar_t message[1025];
		MultiByteToWideChar(CP_ACP, 0, exc.what(), -1, message, 1024);
		MessageBox(NULL, message, NULL, NULL);
	}

	return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  НАЗНАЧЕНИЕ: регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style			= CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc	= WndProc;
	wcex.cbClsExtra		= 0;
	wcex.cbWndExtra		= 0;
	wcex.hInstance		= hInstance;
	wcex.hIcon			= LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WIN32PROJECT3));
	wcex.hCursor		= LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground	= (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName	= NULL;//MAKEINTRESOURCE(IDC_WIN32PROJECT3);
	wcex.lpszClassName	= szWindowClass;
	wcex.hIconSm		= LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

	return RegisterClassEx(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   НАЗНАЧЕНИЕ: сохраняет обработку экземпляра и создает главное окно.
//
//   КОММЕНТАРИИ:
//
//        В данной функции дескриптор экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится на экран главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Сохранить дескриптор экземпляра в глобальной переменной
	hWnd = CreateWindow(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, NULL, NULL, hInstance, NULL);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  НАЗНАЧЕНИЕ:  обрабатывает сообщения в главном окне.
//
//  WM_DESTROY	 - ввести сообщение о выходе и вернуться.
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_KEYDOWN:
		if (!menu->Key(wParam))
		{
			game->Key(wParam);
		}
		break;
	case WM_MOUSEMOVE:
		{
			Point point = gl->UICoordinates(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			if (!menu->Hover(point.X, point.Y))
			{
				point = gl->WorldCoordinates(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				game->Hover(point.X, point.Y);
			}
		}
		break;
	case WM_LBUTTONDOWN:
		{
			Point point = gl->UICoordinates(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			bool handled = menu->Click(point.X, point.Y, 1);
			if (!handled)
			{
				point = gl->WorldCoordinates(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				handled = game->Click(point.X, point.Y, 1);
			}
		}
		break;
	case WM_RBUTTONDOWN:
		{
			Point point = gl->UICoordinates(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
			bool handled = menu->Click(point.X, point.Y, 2);
			if (!handled)
			{
				point = gl->WorldCoordinates(GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam));
				handled = game->Click(point.X, point.Y, 2);
			}
		}
		break;
	case WM_MOUSEWHEEL:
		if (!menu->Scroll(GET_WHEEL_DELTA_WPARAM(wParam)))
		{
			if (!game->Scroll(GET_WHEEL_DELTA_WPARAM(wParam)))
			{
				gl->Scale(GET_WHEEL_DELTA_WPARAM(wParam));
			}
		}
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}
