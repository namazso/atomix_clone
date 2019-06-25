/// @file game.c
/// @author namazso
/// @date 2017-10-11
/// @brief Implements game logic.

#include "pch.h"

#include "globals.h"
#include "sprite_manager.h"
#include "keys.h"
#include "tile_manager.h"
#include "render.h"
#include "map_manager.h"
#include "score_manager.h"
#include "game_modules.h"

/// Current key states.
enum key_state g_key_states[0x100];

/// The displayed bitmap
struct color g_bitmap[k_pixel_width * k_pixel_height];

/// True until the game is running
bool g_running = true;

/// Current game state
struct global_state g_state =
{
	.stage = GameState_Menu
};

/// Font used for writing stuff.
int g_font;

/// Called on game start.
void on_game_start(void)
{
	sprite_manager_init();
	tile_manager_init();
	render_init();
	mapmgr_init();
	scoremgr_init();

	g_font = sprite_manager_load_from_file("font.bin", 128);
}

/// Called on game tick.
void on_game_tick(void)
{
	if(g_key_states[Key_Escape] == KeyState_Pressed)
		g_running = false;

	render_start_frame();

	/*uint8_t time_str[20];
	time_t now = time(NULL);
	strftime((char*)time_str, 32, "%Y-%m-%d %H:%M:%S", localtime(&now));
	render_printf(g_font, 50, 50, "Time: %s", time_str);*/

	switch(g_state.stage)
	{
	case GameState_Menu:
		do_menu();
		break;
	case GameState_Game:
		do_gameplay();
		break;
	case GameState_Highscores:
		do_highscores();
		break;
	default:
		break;
	}


	render_render();
}

/// Called on game end.
void on_game_end(void) {}
