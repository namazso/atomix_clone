/// @file highscore.c
/// @author namazso
/// @date 2017-12-04
/// @brief Highscore stage implementation

#include "pch.h"

#include "game_modules.h"
#include "score.h"
#include "score_manager.h"
#include "game.h"
#include "render.h"

static struct
{
	const struct map* map;
	int score;
	struct score top10[10];
	int topcount;
	int myplace;
} s_state;

void highscore_level_finished(const struct map* map, int score)
{
	if (score < 0)
		score = 0;

	g_state.stage = GameState_Highscores;
	
	s_state.map = map;
	s_state.score = score;
	int count = scoremgr_get_top_10(map, s_state.top10);
	int place = count;
	while(place > 0 && s_state.top10[place - 1].points < (uint32_t)score)
		place--;
	s_state.myplace = place;
	if(place < 10)
	{
		memset(s_state.top10[place].name, ' ', 31);
		s_state.top10[place].name[31] = 0;
		s_state.top10[place].date = time(NULL);
		s_state.top10[place].points = score;
	}
	s_state.topcount = MAX(count, place + 1);
}

static void input_behavior(int x, int y, int* cursor, char* text, int len)
{
	int cur = *cursor;

	const bool is_left_pressed = g_key_states[Key_Left] == KeyState_Pressed;
	const bool is_right_pressed = g_key_states[Key_Right] == KeyState_Pressed;
	const bool is_up_pressed = g_key_states[Key_Up] == KeyState_Pressed;
	const bool is_down_pressed = g_key_states[Key_Down] == KeyState_Pressed;

	if(is_left_pressed) --cur;
	if(is_right_pressed) ++cur;
	CLAMP_IN_PLACE(cur, 0, len);

	if(is_up_pressed) text[cur]++;
	if(is_down_pressed) text[cur]--;
	// Printable ASCII
	CLAMP_IN_PLACE(text[cur], 32, 126);

	render_print(g_font, x, y, text);
	render_print(g_font, x + cur * 8, y + 1, "_");

	*cursor = cur;
}

void do_highscores(void)
{
	draw_background(2);

	for(int i = 0; i < s_state.topcount; ++i)
	{
		char time[9];
		struct tm tm;
		time_t t = s_state.top10[i].date;
		localtime_s(&tm, &t);
		strftime(time, 9, "%D", &tm);
		render_print(g_font, 8, 16 + i * 16, time);

		render_printf(g_font, 8 + 9 * 8, 16 + i * 16, "%05d", s_state.top10[i].points);

		static int cur = 0;

		if(i != s_state.myplace)
			render_print(g_font, 8 + 15 * 8, 16 + i * 16, s_state.top10[i].name);
		else
			input_behavior(8 + 15 * 8, 16 + i * 16, &cur, s_state.top10[i].name, 29);
	}

	if(g_key_states[Key_Return] == KeyState_Pressed)
	{
		if(s_state.myplace < 10)
			scoremgr_add_new_record(s_state.map, s_state.top10[s_state.myplace]);
		gameplay_next_map();
	}
}