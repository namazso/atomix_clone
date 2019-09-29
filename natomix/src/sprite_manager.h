/// @file sprite_manager.h
/// @author namazso
/// @date 2017-10-10
/// @brief Sprite inline implementation and sprite manager headers.

#pragma once
#include "color.h"
#include "globals.h"
#include "growable_buffer2.h"

/// @addtogroup sprites
/// @{

/// A 8x8 32bit RGBA sprite.
struct sprite
{
	/// The actual color values for the pixels.
	struct color pixels[k_sprite_size][k_sprite_size];
};

DEFINE_GROWABLE_BUFFER(struct sprite, sprite_buffer)

/// Loads one or more sprites from a file.
///
/// @param[in] file Path of file to load sprites from.
/// @param[out] dst Target memory.
/// @param[in] count Count of the sprites.
inline void sprite_load_from_file(const char* file, struct sprite* dst,
	int count)
{
	FILE* fp = fopen(file, "rb");
	assert(fp);
	size_t read = fread(dst, sizeof(struct sprite), count, fp);
	assert(read == (size_t)count);
	int close = fclose(fp);
	assert(close == 0);
}

/// Loads sprites from a file that is multiple of k_sprite_size pixels
/// wide.
///
/// @param[in] file Path of file to load sprites from.
/// @param[out] dst Target memory.
/// @param[in] x Vertical count of sprites.
/// @param[in] y Horizontal count of sprites.
inline void sprite_load_from_file_2d(const char* file,
	struct sprite* dst, int x, int y)
{
	struct color* data = (struct color*)malloc(
		x * y * sizeof(struct sprite));
	assert(data);

	FILE* fp = fopen(file, "rb");
	assert(fp);
	size_t read = fread(data, sizeof(struct sprite), x * y, fp);
	assert(read == (size_t)(x * y));
	int close = fclose(fp);
	assert(close == 0);

	// Magic happens here
	for (int i = 0; i < y; ++i)
		for (int j = 0; j < x; ++j)
			for (int k = 0; k < k_sprite_size; ++k)
				for (int l = 0; l < k_sprite_size; ++l)
				{
					const int px =
						((i * k_sprite_size) + k) * (x * k_sprite_size)
						+ j * k_sprite_size + l;

					dst[i * x + j].pixels[k][l] = data[px];
				}

	free(data);
}

/// Draws a sprite onto a bitmap.
///
/// Draws the given sprite onto a given position on the target bitmap
/// with alpha blending.
///
/// @param[in,out] map Target bitmap.
/// @param[in] map_w Map width.
/// @param[in] map_h Map height.
/// @param[in] sprite The sprite to draw.
/// @param[in] x X coordiante of where to draw the sprite.
/// @param[in] y Y coordiante of where to draw the sprite.
inline void sprite_draw_on_bitmap(
	struct color* map, int map_w, int map_h,
	const struct sprite* sprite, int x, int y)
{
	for (int k = 0; k < k_sprite_size; ++k)
	{
		for (int l = 0; l < k_sprite_size; ++l)
		{
			int px_y = y + l;
			int px_x = x + k;
			if (px_y < map_h && px_x < map_w && px_y > 0 && px_x > 0)
			{
				struct color* target = &map[px_y * map_w + px_x];
				*target = color_alpha_blend(*target, sprite->pixels[l][k]);
			}
		}
	}
}

extern void sprite_manager_init(void);

extern int sprite_manager_load_from_file(const char* file, const int count);

extern int sprite_manager_load_from_file_2d(const char* file,
	int x, int y);

extern const struct sprite* sprite_manager_get_by_id(int id);

/// @}
