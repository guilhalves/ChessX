#include "../include/eval.h"
#include "../include/search.h"

int material_score[6] = 
{
	350, // bishop
	500, // rook
	1000, // queen
	100, // pawn
	300, // knight
	10000, // king
};

const int positional_score[6][64] =
{
	// bishop
	{
		0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   0,  10,  10,   0,   0,   0,
		0,   0,  10,  20,  20,  10,   0,   0,
		0,   0,  10,  20,  20,  10,   0,   0,
		0,  10,   0,   0,   0,   0,  10,   0,
		0,  30,   0,   0,   0,   0,  30,   0,
		0,   0, -10,   0,   0, -10,   0,   0
	},

	// rook
	{
		50,  50,  50,  50,  50,  50,  50,  50,
		50,  50,  50,  50,  50,  50,  50,  50,
		0,   0,  10,  20,  20,  10,   0,   0,
		0,   0,  10,  20,  20,  10,   0,   0,
		0,   0,  10,  20,  20,  10,   0,   0,
		0,   0,  10,  20,  20,  10,   0,   0,
		0,   0,  10,  20,  20,  10,   0,   0,
		0,   0,   0,  20,  20,   0,   0,   0
	},

	// queen
	{
		-20,-10,-10, -5, -5,-10,-10,-20,
		-10,  0,  0,  0,  0,  0,  0,-10,
		-10,  0,  5,  5,  5,  5,  0,-10,
		-5,  0,  5,  5,  5,  5,  0, -5,
		0,  0,  5,  5,  5,  5,  0, -5,
		-10,  5,  5,  5,  5,  5,  0,-10,
		-10,  0,  5,  0,  0,  0,  0,-10,
		-20,-10,-10, -5, -5,-10,-10,-20
	},

	// pawn
	{
		90,  90,  90,  90,  90,  90,  90,  90,
		30,  30,  30,  40,  40,  30,  30,  30,
		20,  20,  20,  30,  30,  30,  20,  20,
		10,  10,  10,  20,  20,  10,  10,  10,
		5,   5,  10,  20,  20,   5,   5,   5,
		0,   0,   0,   5,   5,   0,   0,   0,
		0,   0,   0, -10, -10,   0,   0,   0,
		0,   0,   0,   0,   0,   0,   0,   0
	},

	// knight
	{
		-5,   0,   0,   0,   0,   0,   0,  -5,
		-5,   0,   0,  10,  10,   0,   0,  -5,
		-5,   5,  20,  20,  20,  20,   5,  -5,
		-5,  10,  20,  30,  30,  20,  10,  -5,
		-5,  10,  20,  30,  30,  20,  10,  -5,
		-5,   5,  20,  10,  10,  20,   5,  -5,
		-5,   0,   0,   0,   0,   0,   0,  -5,
		-5, -10,   0,   0,   0,   0, -10,  -5
	},

	// king
	{
		0,   0,   0,   0,   0,   0,   0,   0,
		0,   0,   5,   5,   5,   5,   0,   0,
		0,   5,   5,  10,  10,   5,   5,   0,
		0,   5,  10,  20,  20,  10,   5,   0,
		0,   5,  10,  20,  20,  10,   5,   0,
		0,   0,   5,  10,  10,   5,   0,   0,
		0,   5,   5,  -5,  -5,   0,   5,   0,
		0,   0,   5,   0, -15,   0,  10,   0
	}
};

const int mirror_score[128] =
{
	a1, b1, c1, d1, e1, f1, g1, h1,
	a2, b2, c2, d2, e2, f2, g2, h2,
	a3, b3, c3, d3, e3, f3, g3, h3,
	a4, b4, c4, d4, e4, f4, g4, h4,
	a5, b5, c5, d5, e5, f5, g5, h5,
	a6, b6, c6, d6, e6, f6, g6, h6,
	a7, b7, c7, d7, e7, f7, g7, h7,
	a8, b8, c8, d8, e8, f8, g8, h8
};

const int get_column[64] =
{
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7,
	0, 1, 2, 3, 4, 5, 6, 7
};

const int get_row[64] =
{
	7, 7, 7, 7, 7, 7, 7, 7,
	6, 6, 6, 6, 6, 6, 6, 6,
	5, 5, 5, 5, 5, 5, 5, 5,
	4, 4, 4, 4, 4, 4, 4, 4,
	3, 3, 3, 3, 3, 3, 3, 3,
	2, 2, 2, 2, 2, 2, 2, 2,
	1, 1, 1, 1, 1, 1, 1, 1,
	0, 0, 0, 0, 0, 0, 0, 0
};

U64 column_masks[64];

U64 row_masks[64];

U64 isolated_masks[64];

U64 passed_masks[2][64];

const int passed_pawn_bonus[8] = { 0, 10, 30, 50, 75, 100, 150, 200 };

void InitEvaluationMasks()
{
	for (int cell = 0; cell < 64; cell++)
	{
		column_masks[cell] = 0x101010101010101 << get_column[cell];
		row_masks[cell] = 0xff00000000000000 >> (get_row[cell]*8);
	}
	
	for (int cell = 0; cell < 64; cell++)
	{
		if (cell % 8 == 0) isolated_masks[cell] = column_masks[cell+1];
		else if (cell % 8 == 7) isolated_masks[cell] = column_masks[cell-1];
		else isolated_masks[cell] = (column_masks[cell-1] | column_masks[cell+1]);
		
		passed_masks[black][cell] = column_masks[cell];
		passed_masks[black][cell] |= isolated_masks[cell];
		
		passed_masks[white][cell] = column_masks[cell];
		passed_masks[white][cell] |= isolated_masks[cell];

		for (int i = 0; i <= (7-cell/8); i++) passed_masks[white][cell] &= ~row_masks[(7-i)*8+cell%8];
		for (int i = 0; i <= cell/8; i++) passed_masks[black][cell] &= ~row_masks[i*8+cell%8];
	}
}

static inline int EvalBishop(POS *pos, CELL c, bool side)
{
	int score = positional_score[B][(side) ? c : mirror_score[c]];
	score += material_score[B];
	score += (CountBits(GetBishopAttacks(c, pos->occ[black] | pos->occ[white]))-4)*5;
	return((side) ? score : -score);
}
static inline int EvalRook(POS *pos, CELL c, bool side)
{
	int score = positional_score[R][(side) ? c : mirror_score[c]];
	score += material_score[R];
	score += CountBits(GetRookAttacks(c, pos->occ[black] | pos->occ[white]));
	
	if ((pos->bb[Our(P, side)] & column_masks[c]) == 0) score += 10;
	if (((pos->bb[P] | pos->bb[p]) & column_masks[c]) == 0) score += 15;

	return((side) ? score : -score);
}
static inline int EvalQueen(POS *pos, CELL c, bool side)
{
	int score = positional_score[Q][(side) ? c : mirror_score[c]];
	score += material_score[Q];
	score += (CountBits(GetQueenAttacks(c, pos->occ[black] | pos->occ[white]))-9)*1;
	return((side) ? score : -score);
}
static inline int EvalPawn(POS *pos, CELL c, bool side)
{
	int score = positional_score[P][(side) ? c : mirror_score[c]];
	score += material_score[P];
	int double_pawns = CountBits(column_masks[c] & pos->bb[Our(P, side)]);
	
	if (double_pawns > 1) score += (double_pawns-1)*(-7);
	
	if ((pos->bb[Our(P, side)] & isolated_masks[c]) == 0) score += -7;
	
	if ((passed_masks[side][c] & pos->bb[Their(P, side)]) == 0)
	{ score += passed_pawn_bonus[get_row[(side) ? c : mirror_score[c]]]; }

	return((side) ? score : -score);
}
static inline int EvalKnight(POS *pos, CELL c, bool side)
{
	int score = positional_score[N][(side) ? c : mirror_score[c]];
	score += material_score[N];
	
	return((side) ? score : -score);
}
static inline int EvalKing(POS *pos, CELL c, bool side)
{
	int score = positional_score[K][(side) ? c : mirror_score[c]];
	score += material_score[K];
	
	if ((pos->bb[Our(P, side)] & column_masks[c]) == 0) score -= 10;
	if (((pos->bb[P] | pos->bb[p]) & column_masks[c]) == 0) score -= 15;
	
	score += CountBits(KingAttacks[c] & pos->occ[side]) * 5;

	return((side) ? score : -score);
}

static int (*EvalPiece[6])(POS *pos, CELL c, bool side) = { EvalBishop, EvalRook, EvalQueen, EvalPawn, EvalKnight, EvalKing };

int Evaluate(POS *pos)
{
	int score = 0;

	for (CELL c = 0; c < 64; c++)
	{
		if (pos->pt[c] == NO_PIECE) continue;
		
		score += EvalPiece[GetType(pos->pt[c])](pos, c, GetColor(pos->pt[c]));
	}
	
	score = (score * (100-fifty)) / 100;
	return((pos->side) ? score : -score);
}
