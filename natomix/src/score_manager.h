/// @file score_manager.h
/// @author namazso
/// @date 2017-12-04
/// @brief Score manager interface.

#pragma once

#include "score.h"

void scoremgr_init(void);

int scoremgr_get_top_10(const struct map* map, struct score* scores);

void scoremgr_add_new_record(const struct map* map,
	const struct score score);