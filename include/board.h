#ifndef BOARD_H
#define BOARD_H

#include "bitboard.h"
#include <stdbool.h>

enum { B, R, Q, P, N, K, b, r, q, p, n, k, NO_PIECE };

#define GetType(pt) ((pt < b) ? pt : pt - 6)
#define GetColor(pt) ((pt < b) ? white : black)

#define NO_HASH_ENTRY 100000
enum { HASH_FLAG_EXACT, HASH_FLAG_ALPHA, HASH_FLAG_BETA };

typedef struct
{
	U64 bb[12];
	PIECE pt[64]; // piece type matrix
	U64 occ[2];
	
	U64 hash;

	CELL enp;
	int castle;
	
	bool side;
} POS;

typedef struct
{
	U64 hash;
	int depth;
	int flag;
	int score;
	int bestmove;
} TT;

void InitRandomKeys();

void ParseFen(POS *pos, char *fen);

U64 GenerateHashKey(POS* pos);

extern U64 piece_keys[12][64];
extern U64 enpassant_keys[65];
extern U64 castle_keys[16];
extern U64 side_key;

extern int hash_entries;
extern TT *hash_table;

void ClearHashTable();
void InitHashTable(int mb);

#endif
