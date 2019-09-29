/// @file menu.c
/// @author namazso
/// @date 2017-12-04
/// @brief Menu stage implementation

#include "pch.h"

#include "game_modules.h"
#include "game.h"
#include "map_manager.h"
#include "render.h"

static bool do_menu_behavior(const char* const* choices, int count, int* selection, int draw_y)
{
	int select = *selection;
	for(int i = 0; i < count; ++i)
	{
		char selected[k_width_in_sprite + 1];
		const char* to_print;
		if(i == select)
		{
			strcpy(selected, "> ");
			strcat(selected, choices[i]);
			strcat(selected, " <");
			to_print = selected;
		}
		else
		{
			to_print = choices[i];
		}

		int len = (int)strlen(to_print);
		int x = (k_width_in_sprite - len) * k_sprite_size / 2;
		int y = draw_y + (i * 2) * k_sprite_size;
		render_print(g_font, x, y, to_print);
	}

	if(g_key_states[Key_Up] == KeyState_Pressed)
		select--;

	if(g_key_states[Key_Down] == KeyState_Pressed)
		select++;

	select = (select + count) % count;

	*selection = select;

	return g_key_states[Key_Return] == KeyState_Pressed;
}

void do_menu(void)
{
	draw_background(0);

	static int selection = 0;

	static enum
	{
		Menu_Main,
		Menu_PackSelect,
		Menu_HowToPlay
	} state;

	switch(state)
	{
	case Menu_Main:
		{
			static const char* const choices[] =
			{
				"Play",
				"How to play",
				"Quit"
			};
			if(do_menu_behavior(choices, 3, &selection, 60))
			{
				switch(selection)
				{
				case 0:
					state = Menu_PackSelect;
					break;
				case 1:
					state = Menu_HowToPlay;
					break;
				case 2:
					g_running = false;
					break;
				default:
					break;
				}
				selection = 0;
			}
		}
		break;
	case Menu_PackSelect:
		{
			enum { k_max_packs = 12 };
			char choices[k_max_packs][32];
			const char* choices_ptrs[k_max_packs];
			for(int i = 0; i < k_max_packs; ++i)
				choices_ptrs[i] = &choices[i][0];
			const int count = mapmgr_get_pack_names(choices, k_max_packs);
			if(do_menu_behavior(choices_ptrs, count, &selection, 16))
				gameplay_load_map(selection, 0);
		}
		break;
	case Menu_HowToPlay:
		{
			static const char* const choices[] =
			{
				"Atomix takes place on a playfield,",
				"consisting of a number of walls, with",
				"the atoms scattered throughout. You are",
				"tasked with assembling a molecule from",
				"the atoms. The atoms must be arranged",
				"into a specific shape, same as the",
				"shape displayed on the left side of",
				"the screen. Hold space, and press an",
				"arrow to move an atom, but be careful,",
				"because it will slide until it hits a",
				"wall or another atom.",
				"",
				"Back",
			};
			// Allow only `Back`
			selection = 12;
			if(do_menu_behavior(choices, 13, &selection, 16))
			{
				state = Menu_Main;
				selection = 0;
			}
		}
	}
}