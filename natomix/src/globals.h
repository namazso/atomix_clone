/// @file globals.h
/// @author namazso
/// @date 2017-10-10
/// @brief Global constants.
///
/// Global constants of the game.

#pragma once

/// @addtogroup Globals
/// @{

/// Window title.
static const wchar_t* k_window_title = L"nAtomix";

/// Window class name.
static const wchar_t* k_window_class = L"nAtomix";

enum
{
	/// Target bitmap width in pixels.
	k_pixel_width = 320,

	/// Target bitmap height in pixels.
	k_pixel_height = 240,

	/// Game ticks per second.
	k_tickrate = 128,

	/// Height and width of a sprite.
	k_sprite_size = 8,

	/// Target bitmap width in sprites.
	k_width_in_sprite = k_pixel_width / k_sprite_size,

	/// Target bitmap height in sprites.
	k_height_in_sprite = k_pixel_height / k_sprite_size,
};

/// Yay for C constexpr
#define CX_FLAG_TO_ORDER(flag) (\
	!!((flag) & (1 << 0)) * 0 + \
	!!((flag) & (1 << 1)) * 1 + \
	!!((flag) & (1 << 2)) * 2 + \
	!!((flag) & (1 << 3)) * 3 + \
	!!((flag) & (1 << 4)) * 4 + \
	!!((flag) & (1 << 5)) * 5 + \
	!!((flag) & (1 << 6)) * 6 + \
	!!((flag) & (1 << 7)) * 7 + \
	!!((flag) & (1 << 8)) * 8 + \
	!!((flag) & (1 << 9)) * 9 + \
	!!((flag) & (1 << 10)) * 10 + \
	!!((flag) & (1 << 11)) * 11 + \
	!!((flag) & (1 << 12)) * 12 + \
	!!((flag) & (1 << 13)) * 13 + \
	!!((flag) & (1 << 14)) * 14 + \
	!!((flag) & (1 << 15)) * 15 + \
	!!((flag) & (1 << 16)) * 16 + \
	!!((flag) & (1 << 17)) * 17 + \
	!!((flag) & (1 << 18)) * 18 + \
	!!((flag) & (1 << 19)) * 19 + \
	!!((flag) & (1 << 20)) * 20 + \
	!!((flag) & (1 << 21)) * 21 + \
	!!((flag) & (1 << 22)) * 22 + \
	!!((flag) & (1 << 23)) * 23 + \
	!!((flag) & (1 << 24)) * 24 + \
	!!((flag) & (1 << 25)) * 25 + \
	!!((flag) & (1 << 26)) * 26 + \
	!!((flag) & (1 << 27)) * 27 + \
	!!((flag) & (1 << 28)) * 28 + \
	!!((flag) & (1 << 29)) * 29 + \
	!!((flag) & (1 << 30)) * 30 + \
	!!((flag) & (1 << 31)) * 31)

#define MAX(a, b) ((a > b) ? a : b)
#define MIN(a, b) ((a < b) ? a : b)
#define CLAMP(x, a, b) (MIN(MAX(a, x), b))
#define CLAMP_IN_PLACE(x, a, b) (x = CLAMP(x, a, b))

/// @}
