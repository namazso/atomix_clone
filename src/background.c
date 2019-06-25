/// @file background.c
/// @author namazso
/// @date 2017-12-04
/// @brief Background drawer.

#include "pch.h"

#include "sprite_manager.h"
#include "tile_manager.h"
#include "render.h"

static void load_backgrounds(int* bgs, int count)
{
	for(int i = 0; i < count; ++i)
	{
		char path[32];
		sprintf(path, "bg/%d.bin", i);
		bgs[i] = tile_manager_add(
			sprite_manager_load_from_file_2d(path,
				k_width_in_sprite, k_height_in_sprite),
			k_width_in_sprite, k_height_in_sprite);
	}
}

void draw_background(int id)
{
	static int s_bgs[4];
	// Lazy init cuz i'm lazy
	static bool initialized = false;
	if(!initialized)
	{
		load_backgrounds(s_bgs, 4);
		initialized = true;
	}
	render_tile(s_bgs[id], 0, 0);
}
