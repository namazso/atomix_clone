/// @file game.h
/// @author namazso
/// @date 2017-10-10
/// @brief Exports of the game to the platform handling module.

#pragma once
#include "globals.h"
#include "keys.h"
#include "color.h"

extern struct color g_bitmap[k_pixel_width * k_pixel_height];

// Could be atomic_int for more safety, but VS doesnt support it
extern enum key_state g_key_states[0x100];

extern bool g_running;

/// Check if a key is held down.
///
/// @param[in] key Key to check the state of.
/// @return True if key is down, false otherwise.
inline bool is_key_down(enum key_code key)
{
	return !!(g_key_states[(int)key] & KeyFlag_PushState);
}

extern void on_game_start(void);

extern void on_game_tick(void);

extern void on_game_end(void);
