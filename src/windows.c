/// @file windows.c
/// @author namazso
/// @date 2017-10-09
/// @brief Handles interactions between the game and Windows
///
/// This file registers and creates the window, initializes the bitmap
/// used for drawing, and takes care of starting threads.

#include "pch.h"

#define PSAPI_VERSION 2
#define WIN32_LEAN_AND_MEAN
#define NOIME
#define NOWINRES
#define NOGDICAPMASKS
#define NOVIRTUALKEYCODES
//#define NOWINMESSAGES
//#define NOWINSTYLES
#define NOSYSMETRICS
#define NOMENUS
#define NOICONS
#define NOKEYSTATES
#define NOSYSCOMMANDS
//#define NORASTEROPS
#define NOSHOWWINDOW
#define OEMRESOURCE
#define NOATOM
#define NOCLIPBOARD
//#define NOCOLOR
#define NOCTLMGR
#define NODRAWTEXT
//#define NOGDI
//#define NOUSER
#define NOMB
#define NOMEMMGR
#define NOMETAFILE
#define NOMINMAX
//#define NOMSG
#define NOOPENFILE
#define NOSCROLL
#define NOSERVICE
#define NOSOUND
#define NOTEXTMETRIC
#define NOWH
#define NOWINOFFSETS
#define NOCOMM
#define NOKANJI
#define NOHELP
#define NOPROFILER
#define NODEFERWINDOWPOS
#define NOMCX
#define NOIME
#define NOPROXYSTUB
#define NOIMAGE
#define NO
#define NOTAPE
#define ANSI_ONLY
#include <windows.h>

#include "globals.h"
#include "game.h"

/// @addtogroup windows
/// @{

/// The program instance.
static HINSTANCE s_instance;

/// The window
static HWND s_hwnd;

/// Holds the multiplier for drawing, so we can play in normal sizes.
static int s_display_size_multiplier = 4;

/// True until our game is running
static bool s_is_running = true;

/// Get the actual rendered window width.
inline int get_window_width(void)
{
	return s_display_size_multiplier * k_pixel_width;
}

/// Get the actual rendered window height.
inline int get_window_height(void)
{
	return s_display_size_multiplier * k_pixel_height;
}

// Forward declarations
static ATOM register_class(HINSTANCE instance);
static BOOL init_instance(HINSTANCE, int);
static LRESULT CALLBACK window_process(HWND, UINT, WPARAM, LPARAM);

/// Application entry point.
///
/// Used to initialize a window that displays our global
/// bitmap.
///
/// @param[in] instance The program instance.
/// @param[in] prev_instance The previous program instance. Optional.
/// @param[in] cmd_line Command line parameters.
/// @param[in] cmd_show Window show type.
/// @return The w_param of the received WM_QUIT if succeeded
int APIENTRY wWinMain(HINSTANCE instance,
	HINSTANCE prev_instance,
	LPWSTR cmd_line,
	int cmd_show)
{
	UNREFERENCED_PARAMETER(prev_instance);
	UNREFERENCED_PARAMETER(cmd_line);

	// Initialize global strings
	register_class(instance);

	// Perform application initialization:
	BOOL result = init_instance(instance, cmd_show);
	assert(result);
	if (!result)
		return FALSE;


	// Lets figure out QueryPerformanceCounter resolution
	long long one_millisecond;
	{
		LARGE_INTEGER first;
		result = QueryPerformanceCounter(&first);
		assert(result);
		Sleep(160);
		LARGE_INTEGER second;
		result = QueryPerformanceCounter(&second);
		assert(result);
		one_millisecond = (second.QuadPart - first.QuadPart) / 160;
	}

	on_game_start();

	MSG msg;
	msg.wParam = 0;
	while (g_running)
	{
		LARGE_INTEGER frame_begin_time;
		result = QueryPerformanceCounter(&frame_begin_time);
		assert(result);

		// Clear the fresh state change flags from the keys
		for (int i = 0; i < 0x100; ++i)
			g_key_states[i] &= KeyFlag_PushState;

		// Main message loop:
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		if (!s_is_running)
			break;

		on_game_tick();

		// For whatever reason MS uses BGRA
		for(int i = 0; i < 320 * 240; ++i)
		{
			struct color rgba = g_bitmap[i];
			struct color bgra = color_rgba(rgba.b, rgba.g, rgba.r, rgba.a);
			g_bitmap[i] = bgra;
		}

		result = RedrawWindow(s_hwnd, NULL, NULL, RDW_FRAME | RDW_INVALIDATE);
		assert(result);

		// Lets burn some cpu
		{
			LARGE_INTEGER current_time;
			do
				QueryPerformanceCounter(&current_time);
			while (current_time.QuadPart < frame_begin_time.QuadPart +
				one_millisecond * 1000 / k_tickrate);
		}
	}

	on_game_end();

	return (int)msg.wParam;
}

/// Registers the window class.
///
/// Sets icons, title and class name.
///
/// @param[in] instance The program instance
/// @return The registered class atom
static ATOM register_class(HINSTANCE instance)
{
	WNDCLASSEX wcex;

	wcex.cbSize = sizeof(WNDCLASSEX);

	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = window_process;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = instance;
	wcex.hIcon = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = NULL;
	wcex.lpszClassName = (LPCWSTR)k_window_class;
	wcex.hIconSm = NULL;

	ATOM atom = RegisterClassEx(&wcex);
	assert(atom);

	return atom;
}

/// Saves instance handle and creates main window.
///
/// In this function, we save the instance handle in a global variable and
/// create and display the main program window.
///
/// @param[in] instance The program instance
/// @param[in] cmd_show Window showing type.
/// @return TRUE if succeeded, FALSE otherwise
static BOOL init_instance(HINSTANCE instance, int cmd_show)
{
	s_instance = instance; // Store instance handle in our global variable

	s_hwnd = CreateWindow((LPCWSTR)k_window_class, (LPCWSTR)
		k_window_title, WS_POPUP,
		CW_USEDEFAULT, 0, get_window_width(), get_window_height(), NULL, NULL,
		instance, NULL);

	assert(s_hwnd);
	if (!s_hwnd)
		return FALSE;

	ShowWindow(s_hwnd, cmd_show);

	BOOL result = UpdateWindow(s_hwnd);
	assert(result);

	return TRUE;
}

/// Handles drawing the global bitmap onto the window HDC.
///
/// Called on every window update. Creates a windows bitmap, renders
/// the global bitmap onto it, then copies it over to the window's
/// HDC
///
/// @param[in] hdc HDC to draw onto
static void WINAPI custom_drawer(HDC hdc)
{
	const HDC mem_hdc = CreateCompatibleDC(NULL);
	assert(mem_hdc);
	//const HBITMAP bmp = CreateCompatibleBitmap(hdc, width, height);
	const HBITMAP bmp = CreateBitmap(
		k_pixel_width, k_pixel_height, 1, 32, g_bitmap);
	assert(bmp);

	SelectObject(mem_hdc, bmp);

	BOOL result = StretchBlt(hdc, 0, 0,
		get_window_width(), get_window_height(), mem_hdc,
		0, 0, k_pixel_width, k_pixel_height, SRCCOPY);
	assert(result);

	result = DeleteObject(bmp);
	assert(result);
	result = DeleteDC(mem_hdc);
	assert(result);
}

/// Processes key state changes.
///
/// Sets changed state unconditionally, and down state if the key
/// was pressed.
///
/// @param[in] key Key to change state of.
/// @param[in] pressed Whether it was pressed or released.
void process_key_state_change(int key, bool pressed)
{
	g_key_states[key & 0xFF] = pressed | KeyFlag_ChangedState;
}

/// Figures out what keys got pressed from the window event.
///
/// @param[in] message The received message code.
/// @param[in] w_param Parameter dependant on message.
/// @param[in] l_param Parameter dependant on message.
void update_key_states(UINT message, WPARAM w_param, LPARAM l_param)
{
	bool down = false;

	switch (message)
	{
	case WM_KEYDOWN:
	case WM_KEYUP:
	case WM_SYSKEYDOWN:
	case WM_SYSKEYUP:
		down = !((l_param >> 31) & 1);
		process_key_state_change((int)w_param, down);
		break;

	case WM_LBUTTONDOWN:
		down = true;
	case WM_LBUTTONUP:
		process_key_state_change(Key_LeftButton, down);
		break;

	case WM_RBUTTONDOWN:
		down = true;
	case WM_RBUTTONUP:
		process_key_state_change(Key_RightButton, down);
		break;

	case WM_MBUTTONDOWN:
		down = true;
	case WM_MBUTTONUP:
		process_key_state_change(Key_MiddleButton, down);
		break;

	case WM_XBUTTONDOWN:
		down = true;
	case WM_XBUTTONUP:
		process_key_state_change(
			GET_XBUTTON_WPARAM(w_param) == 1
				? Key_ExtraButton1
				: Key_ExtraButton2, down);
		break;
	default:
		break;
	}
}

/// Handles window events.
///
/// Handles rendering, moving, and keypresses.
/// 
/// @param[in] hwnd Window which the message happened to.
/// @param[in] message The received message code.
/// @param[in] w_param Parameter dependant on message.
/// @param[in] l_param Parameter dependant on message.
/// @return Dependant on message.
static LRESULT CALLBACK window_process(HWND hwnd,
	UINT message,
	WPARAM w_param,
	LPARAM l_param)
{
	update_key_states(message, w_param, l_param);

	switch (message)
	{
	case WM_PAINT:
		{
			PAINTSTRUCT ps;
			const HDC hdc = BeginPaint(hwnd, &ps);
			assert(hdc);
			custom_drawer(hdc);
			BOOL result = EndPaint(hwnd, &ps);
			assert(result);
		}
		break;

	case WM_DESTROY:
		s_is_running = false;
		PostQuitMessage(0);
		break;

	case WM_NCHITTEST:
		// Handle moving window by clicking anywhere.
		// Thanks, stackoverflow
		return HTCAPTION;

	default:
		return DefWindowProc(hwnd, message, w_param, l_param);
	}
	return 0;
}

/// @}