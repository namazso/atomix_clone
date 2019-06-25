/// @file score_manager.c
/// @author namazso
/// @date 2017-12-04
/// @brief Score manager. Identifies maps by their hashes.

#include "pch.h"

#include "fnv.h"
#include "map.h"
#include "score.h"
#include "growable_buffer2.h"

struct score_record
{
	fnv_t hash;
	struct score value;
};

DEFINE_GROWABLE_BUFFER(struct score_record, score_record_buffer)
static struct score_record_buffer s_scores;

// Perfect code
static fnv_t uid_of_map(const struct map* const map)
{
	fnv_t fnv;
	fnv_init(&fnv);
	fnv_hash(&fnv, map->molecule, sizeof(map->molecule));
	fnv_hash(&fnv, map->arena, sizeof(map->arena));
	return fnv;
}

void scoremgr_init(void)
{
	score_record_buffer_init(&s_scores);
	FILE* fp = fopen("highscores.bin", "rb");
	if (!fp)
	{
		fp = fopen("highscores.bin", "wb");
		assert(fp);
		int result = fclose(fp);
		assert(result == 0);
		fp = fopen("highscores.bin", "rb");
	}
	assert(fp);
	int result = fseek(fp, 0L, SEEK_END);
	assert(result == 0);
	const int size = ftell(fp);
	const size_t count = (size_t)size / sizeof(struct score_record);
	rewind(fp);
	score_record_buffer_resize(&s_scores, (int)count);
	const size_t result_c = fread(s_scores.mem, sizeof(struct score_record), count, fp);
	assert(result_c == count);
	result = fclose(fp);
	assert(result == 0);
}

int score_record_comparor(const void* a, const void* b)
{
	struct score_record* r_a = (struct score_record*)a;
	struct score_record* r_b = (struct score_record*)b;
	return r_a->hash > r_b->hash ? 1
		: r_a->hash == r_b->hash ? 0 : -1;
}

int scoremgr_get_top_10(const struct map* map, struct score* scores)
{
	const fnv_t hash = uid_of_map(map);
	struct score_record record;
	record.hash = hash;
	const void* const result = bsearch(&record, s_scores.mem, s_scores.size,
		sizeof(struct score_record), &score_record_comparor);
	int count = 0;
	if(result)
	{
		const struct score_record* first = (struct score_record*)(result);
		while (first > s_scores.mem && (first - 1)->hash == hash)
			first--;
		for (; count < 10 && first[count].hash == hash; count++)
			scores[count] = first[count].value;
	}
	return count;
}

void scoremgr_add_new_record(const struct map* const map,
	const struct score score)
{
	struct score_record record;
	record.hash = uid_of_map(map);
	record.value = score;
	score_record_buffer_push(&s_scores, &record);
	qsort(s_scores.mem, s_scores.size, sizeof(struct score_record),
		&score_record_comparor);
	FILE* fp = fopen("highscores.bin", "wb");
	fwrite(s_scores.mem, sizeof(struct score_record), s_scores.size, fp);
	fclose(fp);
}