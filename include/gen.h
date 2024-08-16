#ifndef GEN_H
#define GEN_H

#include "bitboard.h"
#include "board.h"
#include "attacks.h"

#define GetTo(m) ((m) & 0x3f)
#define GetFrom(m) (((m) >> 6) & 0x3f)
#define GetFlag(m) (((m) >> 12) & 0x3f)
#define Our(pt, c) ((c) ? (pt) : (pt) + 6)
#define Their(pt, c) ((c) ? (pt + 6) : (pt))

enum {
	QUIET = 0b0000, DOUBLE_PUSH = 0b0001,
	OO = 0b0010, OOO = 0b0011,
	CAPTURE = 0b1000,
	EN_PASSANT = 0b1010,
	PROMOTION = 0b0100,
	PROMOTION_CAPTURES = 0b1100,
	PR_KNIGHT = 0b0100, PR_BISHOP = 0b0101, PR_ROOK = 0b0110, PR_QUEEN = 0b0111,
	PC_KNIGHT = 0b1100, PC_BISHOP = 0b1101, PC_ROOK = 0b1110, PC_QUEEN = 0b1111,
};


enum { wk = 1, wq = 2, bk = 4, bq = 8 };

typedef struct
{
	int moves[256];
	int end;
} LIST;

U64 GetBishopAttacks(CELL cell, U64 occupancy);
U64 GetRookAttacks(CELL cell, U64 occupancy);
U64 GetQueenAttacks(CELL cell, U64 occupancy);

void GenMoves(POS *pos, LIST *list);

void GenMovesQSC(POS *pos, LIST *list);

int IsCellAttacked(POS *pos, CELL cell, int attacker_side);

int MakeMove(POS *pos, int move);

#endif
