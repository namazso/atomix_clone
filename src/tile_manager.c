/// @file tile_manager.c
/// @author namazso
/// @date 2017-10-11
/// @brief Tile manager implementation.

#include "pch.h"

#include "growable_buffer2.h"
#include "tile_manager.h"

/// @addtogroup tiles
/// @{

DEFINE_GROWABLE_BUFFER(struct tile, tile_buffer);

static struct tile_buffer s_tiles;

/// Initialize the tile manager.
void tile_manager_init(void)
{
	tile_buffer_init(&s_tiles);
}

/// Add a tile to the tile manager.
///
/// Create a new tile and return its id.
///
/// @param[in] start Starting sprite id.
/// @param[in] width Width of the new tile in sprites.
/// @param[in] height Height of the new tile in sprites.
/// @return ID of the newly added tile.
int tile_manager_add(int start, int width, int height)
{
	struct tile tile = tile_create(start, width, height);
	return tile_buffer_push(&s_tiles, &tile);
}

/// Get a tile by id.
///
/// Get a pointer to the tile identified by ID.
///
/// @param[in] id The id of the tile.
/// @return Pointer to the tile.
/// @warning The pointer may become invalid when adding a new tile.
const struct tile* tile_manager_get_by_id(int id)
{
	return &(s_tiles.mem)[id];
}

/// @}
