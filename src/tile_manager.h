/// @file tile_manager.h
/// @author namazso
/// @date 2017-10-11
/// @brief Tile inline implementation and tile manager headers.

#pragma once
#include "sprite_manager.h"

/// @addtogroup tiles
/// @{

/// A tile made of one or more sprites.
struct tile
{
	/// Sprite id where the tile starts.
	int start_id;

	/// Number of sprites in a row.
	int width;

	/// Number of sprites in a column.
	int height;
};

/// Creates a tile.
///
/// @param[in] start_id Sprite manager id of first sprite
/// @param[in] width Width in sprites
/// @param[in] height Height in sprites
/// @return The tile.
inline struct tile tile_create(int start_id, int width, int height)
{
	struct tile t;
	t.start_id = start_id;
	t.width = width;
	t.height = height;
	return t;
}

/// Draws a tile on a bitmap
///
/// Draws the tile on the bitmap on a given position
///
/// @param[in,out] map The bitmap to draw to.
/// @param[in] map_w Width of the bitmap.
/// @param[in] map_h Height of the bitmap.
/// @param[in] tile Pointer to the tile to draw.
/// @param[in] x Vertical position to draw to.
/// @param[in] y Horizontal position to draw to.
inline void tile_draw_on_bitmap(struct color* map, int map_w, int map_h,
	const struct tile* tile, int x, int y)
{
	for (int i = 0; i < tile->width; ++i)
		for (int j = 0; j < tile->height; ++j)
			sprite_draw_on_bitmap(map, map_w, map_h,
				sprite_manager_get_by_id(tile->start_id + j * tile->width + i),
				x + i * k_sprite_size, y + j * k_sprite_size);
}

extern void tile_manager_init(void);

extern int tile_manager_add(int start, int width, int height);

extern const struct tile* tile_manager_get_by_id(int id);

/// @}
