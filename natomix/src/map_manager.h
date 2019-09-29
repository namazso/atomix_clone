/// @file map_manager.h
/// @author namazso
/// @date 2017-12-04
/// @brief Map manager header

#pragma once
#include "map.h"

extern void mapmgr_init(void);

extern int mapmgr_get_pack_names(char packs[][32], int size);

extern const struct map* mapmgr_get_pack_level(int packid, int id);
