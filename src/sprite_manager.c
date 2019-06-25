/// @file sprite_manager.c
/// @author namazso
/// @date 2017-10-10
/// @brief Sprite manager implementation.

#include "pch.h"

#include "sprite_manager.h"
#include "growable_buffer2.h"

/// @addtogroup sprites
/// @{

static struct sprite_buffer s_sprites;

/// Initializes the global sprite manager.
void sprite_manager_init(void)
{
	sprite_buffer_init(&s_sprites);
}

/// Loads one or more sprites from a file into the manager.
///
/// @param[in] file Name of the file to load
/// @param[in] count Count of sprites in the file
/// @return First loaded sprite ID
int sprite_manager_load_from_file(const char* file, const int count)
{
	const int new_sprites = sprite_buffer_grow(&s_sprites, (int)count);
	sprite_load_from_file(file,
		&s_sprites.mem[new_sprites], (int)count);
	return new_sprites;
}

/// Loads sprites from a file that is multiple of k_sprite_size pixels
/// wide.
///
/// @param[in] file Path of file to load sprites from.
/// @param[in] x Vertical count of sprites.
/// @param[in] y Horizontal count of sprites.
/// @return First loaded sprite ID
int sprite_manager_load_from_file_2d(const char* file, int x, int y)
{
	const int count = x * y;
	const int new_sprites = sprite_buffer_grow(&s_sprites, (int)count);
	sprite_load_from_file_2d(file,
		&s_sprites.mem[new_sprites], x, y);
	return new_sprites;
}

/// Returns the sprite associated to the given ID.
///
/// @param[in] id The ID of the sprite.
/// @return Pointer to the sprite struct.
/// @warning The pointer may become invalid after loading new sprites.
const struct sprite* sprite_manager_get_by_id(int id)
{
	return &s_sprites.mem[id];
}

/// @}
