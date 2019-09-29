/// @file atom.c
/// @author namazso
/// @date 2017-12-04
/// @brief Atom manager.

#include "pch.h"

#include "render.h"
#include "tile_manager.h"
#include "map.h"

static void load_atoms(int atom_tiles[128], int bond_tiles[16])
{
	const int bond_index = sprite_manager_load_from_file_2d("bonds.bin", 2, 2 * 16);
	for(int i = 0; i < 16; ++i)
		bond_tiles[i] = tile_manager_add(bond_index + i * 2 * 2, 2, 2);

	#define LoadItem(name) atom_tiles[Item_ ## name] = tile_manager_add(sprite_manager_load_from_file_2d("items/" #name ".bin", 2, 2), 2, 2)

	// Add atoms here
	LoadItem(AtomHydrogen);
	LoadItem(AtomOxygen);
	LoadItem(AtomCarbon);
	LoadItem(AtomFluorine);
	LoadItem(Wall);

	#undef LoadItem
}

void draw_atom(const struct atom* atom, int x, int y)
{
	static int s_atom_tiles[128];
	static int s_bond_tiles[16];

	// Lazy init cuz i'm lazy
	static bool initialized = false;
	if(!initialized)
	{
		initialized = true;
		load_atoms(s_atom_tiles, s_bond_tiles);
	}

	// Dont try to draw air
	if(atom->item_kind)
	{
		for(int i = 0; i < 16; ++i)
			if(atom->bond_flags & (1 << i))
				render_tile(s_bond_tiles[i], x, y);

		int atom_tile = s_atom_tiles[atom->item_kind];
 		assert(atom_tile);
		render_tile(atom_tile, x, y);
	}
}
