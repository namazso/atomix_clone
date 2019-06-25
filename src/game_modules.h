/// @file game_modules.h
/// @author namazso
/// @date 2017-12-04
/// @brief Forward definitions of exposed parts of the game modules

#pragma once
#include "map.h"

extern void draw_atom(const struct atom* atom, int x, int y);

extern void draw_background(int id);

extern void gameplay_load_map(int pack, int lvl);

extern void gameplay_next_map(void);

extern void do_menu(void);

extern void do_gameplay(void);

extern void do_highscores(void);

extern void highscore_level_finished(const struct map* map, int score);

enum game_stage
{
	GameState_Menu,
	GameState_Highscores,
	GameState_Game
};

struct global_state
{
	enum game_stage stage;
};

extern struct global_state g_state;

extern int g_font;