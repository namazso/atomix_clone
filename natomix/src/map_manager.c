/// @file map_manager.c
/// @author namazso
/// @date 2017-12-04
/// @brief Map manager.

#include "pch.h"

#include "map.h"

struct pack_head
{
	char name[32];
	int count;
	struct map* levels;
};

static struct
{
	int count;
	struct pack_head* packs;
} s_packs;

void mapmgr_init(void)
{
	FILE* fp = fopen("packs.map", "rb");
	uint32_t packcount;
	fread(&packcount, sizeof(packcount), 1, fp);
	s_packs.count = packcount;
	s_packs.packs = malloc(sizeof(*s_packs.packs) * packcount);
	for(int i = 0; i < (int)packcount; ++i)
	{
		char name[32];
		uint32_t levelcount;
		fread(name, 32, 1, fp);
		strcpy(s_packs.packs[i].name, name);
		fread(&levelcount, sizeof(levelcount), 1, fp);
		s_packs.packs[i].count = levelcount;
		s_packs.packs[i].levels = malloc(
			sizeof(*s_packs.packs[i].levels) * levelcount);
		fread(s_packs.packs[i].levels, sizeof(*s_packs.packs[i].levels),
			levelcount, fp);
	}
}

int mapmgr_get_pack_names(char packs[][32], const int size)
{
	const int count = min(s_packs.count, size);
	for(int i = 0; i < count; ++i)
		strcpy_s(packs[i], 32, s_packs.packs[i].name);
	return count;
}

const struct map* mapmgr_get_pack_level(const int packid, const int id)
{
	return s_packs.packs[packid].count > id ? &s_packs.packs[packid].levels[id] : NULL;
}
