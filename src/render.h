/// @file render.h
/// @author namazso
/// @date 2017-10-11
/// @brief Render interface headers.

#pragma once
#include "globals.h"

/// @addtogroup render
/// @{

extern void render_init(void);

extern void render_start_frame(void);

extern void render_sprite(int id, int x, int y);

extern void render_tile(int id, int x, int y);

extern void render_print(int font_spr, int x, int y, const char* str);

extern void render_printf(int font_spr, int x, int y,
	const char* fmt, ...);

extern void render_render(void);

/// @}
