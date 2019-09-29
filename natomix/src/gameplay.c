/// @file gameplay.c
/// @author namazso
/// @date 2017-12-04
/// @brief Gameplay stage implementation

#include "pch.h"

#include "game_modules.h"
#include "map_manager.h"
#include "game.h"
#include "tile_manager.h"
#include "render.h"

enum direction
{
	Direction_Left,
	Direction_Right,
	Direction_Up,
	Direction_Down
};

/// Translate direction enum to 2d 1 or -1 s
static inline void direction_to_xy(enum direction dir, int* x, int* y)
{
	switch(dir)
	{
	case Direction_Left:	*x = -1;	*y = 0;		break;
	case Direction_Right:	*x = 1;		*y = 0;		break;
	case Direction_Up:		*x = 0;		*y = -1;	break;
	case Direction_Down:	*x = 0;		*y = 1;		break;
	}
}

/// Floor based on another floats sign
static inline float signfloorf(float val, float sign)
{
	const float multip = copysignf(1, sign);
	return floorf(multip * val) * multip;
}

static struct
{
	const struct map* original_map;
	struct map map;
	int packid;
	int level;
	int score;
	time_t end_time;

	struct
	{
		char id;
		float x;
		float y;
		enum direction direction;
	} current_atom;

	struct
	{
		int x;
		int y;
	} cursor;
} s_state;

void gameplay_load_map(int pack, int lvl)
{
	g_state.stage = GameState_Game;

	s_state.end_time = time(NULL) + 3 * 60;
	s_state.score = 10000;
	s_state.packid = pack;
	s_state.level = lvl;
	s_state.original_map = mapmgr_get_pack_level(pack, lvl);
	s_state.map = *s_state.original_map;
}

void gameplay_next_map(void)
{
	gameplay_load_map(s_state.packid, s_state.level + 1);
	if(!s_state.original_map)
		g_state.stage = GameState_Menu;
}

static void draw_cursor(int x, int y)
{
	static bool initialized = false;
	static int cur;
	if(!initialized)
	{
		initialized = true;
		cur = tile_manager_add(sprite_manager_load_from_file_2d("cursor.bin", 2, 2), 2, 2);
	}
	render_tile(cur, x, y);
}

static void process_movement(void)
{
	if(s_state.current_atom.id)
	{
		float x = s_state.current_atom.x;
		float y = s_state.current_atom.y;
		int move_x;
		int move_y;
		direction_to_xy(s_state.current_atom.direction, &move_x, &move_y);
		s_state.current_atom.x += move_x * 0.07f;
		s_state.current_atom.y += move_y * 0.07f;
		int round_x = (int)signfloorf(x, (float)move_x);
		int round_y = (int)signfloorf(y, (float)move_y);
		int next_x = round_x + move_x;
		int next_y = round_y + move_y;

		// Make sure we are close enough
		double diff = fabs((x - (float)round_x) + (y - (float)round_y));

		// We hit something
		if(s_state.map.arena[next_x][next_y] && diff < 0.15)
		{
			s_state.map.arena[round_x][round_y] = s_state.current_atom.id;
			s_state.current_atom.id = 0;
			s_state.cursor.x = round_x;
			s_state.cursor.y = round_y;
		}
	}
	else
	{
		const bool is_left_pressed = g_key_states[Key_Left] == KeyState_Pressed;
		const bool is_right_pressed = g_key_states[Key_Right] == KeyState_Pressed;
		const bool is_up_pressed = g_key_states[Key_Up] == KeyState_Pressed;
		const bool is_down_pressed = g_key_states[Key_Down] == KeyState_Pressed;
		const bool is_space_held = g_key_states[Key_Space] & KeyFlag_PushState;

		const int x = s_state.cursor.x;
		const int y = s_state.cursor.y;

		if(is_space_held)
		{
			bool is_x = is_left_pressed || is_right_pressed;
			bool is_y = is_down_pressed || is_up_pressed;

			if((is_x || is_y) && s_state.map.arena[x][y]
				&& s_state.map.arena[x][y] != '#')
			{
				s_state.score -= 50;
				s_state.current_atom.id = s_state.map.arena[x][y];
				s_state.current_atom.x = (float)x;
				s_state.current_atom.y = (float)y;
				s_state.map.arena[x][y] = 0;
				s_state.current_atom.direction =
					is_left_pressed ? Direction_Left :
					is_right_pressed ? Direction_Right :
					is_up_pressed ? Direction_Up :
					is_down_pressed ? Direction_Down : -1;
			}
		}
		else
		{
			s_state.cursor.x -= (int)is_left_pressed;
			s_state.cursor.x += (int)is_right_pressed;
			s_state.cursor.y -= (int)is_up_pressed;
			s_state.cursor.y += (int)is_down_pressed;
			CLAMP_IN_PLACE(s_state.cursor.x, 0, 31);
			CLAMP_IN_PLACE(s_state.cursor.y, 0, 31);
		}
	}
}

static void render_mapview(int x, int y, int w, int h)
{
	const int cur_x = s_state.cursor.x;
	const int cur_y = s_state.cursor.y;
	int map_dpos_x = cur_x - w / 2;
	int map_dpos_y = cur_y - h / 2;
	CLAMP_IN_PLACE(map_dpos_x, 0, 32 - w);
	CLAMP_IN_PLACE(map_dpos_y, 0, 32 - h);
	for(int i = 0; i < w; ++i)
		for(int j = 0; j < h; ++j)
		{
			const char atom_id = s_state.map.arena[map_dpos_x + i][map_dpos_y + j];
			const static struct atom k_wall = { .bond_flags = 0, .item_kind = '#' };
			const struct atom* atom = atom_id == '#' ? &k_wall : &s_state.map.atoms[atom_id];
			draw_atom(atom, (x + i) * k_sprite_size * 2, (y + j) * k_sprite_size * 2);
		}

	if(s_state.current_atom.id)
		draw_atom(&s_state.map.atoms[s_state.current_atom.id],
			(int)((s_state.current_atom.x + x - map_dpos_x) * k_sprite_size * 2),
			(int)((s_state.current_atom.y + y - map_dpos_y) * k_sprite_size * 2));
	else
		draw_cursor(
			(x + cur_x - map_dpos_x) * k_sprite_size * 2,
			(y + cur_y - map_dpos_y) * k_sprite_size * 2);
}

static bool check_win_conditions(void)
{
	for(int i = 0; i < 32; ++i)
		for(int j = 0; j < 32; ++j)
		{
			bool match = true;
			for(int k = 0; k < 16 && match; ++k)
				for(int l = 0; l < 16 && match; ++l)
					if(s_state.map.molecule[k][l])
						if(i + k >= 32 || j + l >= 32
							|| s_state.map.arena[i + k][j + l]
								!= s_state.map.molecule[k][l])
							match = false;
			if(match)
				return true;
		}

	return false;
}

void do_gameplay(void)
{
	draw_background(3);

	const size_t len = strlen(s_state.map.name);
	render_print(g_font, (k_pixel_width - (int)len * k_sprite_size) / 2, 16,
		s_state.map.name);

	const time_t left = s_state.end_time - time(NULL);
	render_printf(g_font, 16, 80, "Time left: %01d:%02d",
		left / 60, left % 60);

	render_printf(g_font, 16, 112, "Score: %05d", s_state.score);

	if(check_win_conditions())
		highscore_level_finished(s_state.original_map, s_state.score + (int)left * 3);

	process_movement();

	render_mapview(10, 5, 10, 10);
}