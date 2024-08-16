#ifndef SEARCH_H
#define SEARCH_H

#include "bitboard.h"
#include "board.h"
#include "gen.h"
#include "eval.h"

#define MAX_PLY 99

typedef struct
{
	int length;
	int pv[MAX_PLY];
} LINE;

void SearchBest(POS *pos, int depth);

extern int repetition_index;
extern int fifty;
extern LINE pv_line;

#endif
