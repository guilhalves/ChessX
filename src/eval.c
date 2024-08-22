#include "../include/eval.h"
#include "../include/search.h"

#define DPAWN_PENALTY_OP -5
#define DPAWN_PENALTY_END -10
#define ISOLATED_PAWN_OP -5
#define ISOLATED_PAWN_END -10
#define SEMIOPEN_COLUMN_BONUS 10
#define OPEN_COLUMN_BONUS 15
#define BISHOP_UNIT 4
#define QUEEN_UNIT 9
#define BISHOP_MOB_OP 5
#define BISHOP_MOB_END 5
#define QUEEN_MOB_OP 1
#define QUEEN_MOB_END 2
#define KING_SAFETY_BONUS 5
#define CONNECTED_ROOKS_BONUS 8
#define OPENING_PHASE_SCORE 6192
#define ENDGAME_PHASE_SCORE 518

int material_score[2][6] = 
{
	{
	365, // opening bishop
	477, // opening rook
	1025,// opening queen
	82,  // opening pawn
	337, // opening knight
	0    // opening king
	},
	{
	297, // endgame bishop
	512, // endgame rook
	936, // endgame queen
	94,  // endgame pawn
	281, // endgame knight
	0    // endgame king
	}
};

const int positional_score[2][6][64] =
{
	// opening tables
	{
	// bishop
	{
	    -29,   4, -82, -37, -25, -42,   7,  -8,
	    -26,  16, -18, -13,  30,  59,  18, -47,
   	    -16,  37,  43,  40,  35,  50,  37,  -2,
	     -4,   5,  19,  50,  37,  37,   7,  -2,
 	     -6,  13,  13,  26,  34,  12,  10,   4,
  	      0,  15,  15,  15,  14,  27,  18,  10,
   	      4,  15,  16,   0,   7,  21,  33,   1,
    	-33,  -3, -14, -21, -13, -12, -39, -21,
	},

	// rook
	{
	     32,  42,  32,  51, 63,  9,  31,  43,
	     27,  32,  58,  62, 80, 67,  26,  44,
	     -5,  19,  26,  36, 17, 45,  61,  16,
	    -24, -11,   7,  26, 24, 35,  -8, -20,
	    -36, -26, -12,  -1,  9, -7,   6, -23,
	    -45, -25, -16, -17,  3,  0,  -5, -33,
	    -44, -16, -20,  -9, -1, 11,  -6, -71,
	    -19, -13,   1,  17, 16,  7, -37, -26,
	},

	// queen
	{
	    -28,   0,  29,  12,  59,  44,  43,  45,
	    -24, -39,  -5,   1, -16,  57,  28,  54,
	    -13, -17,   7,   8,  29,  56,  47,  57,
	    -27, -27, -16, -16,  -1,  17,  -2,   1,
	     -9, -26,  -9, -10,  -2,  -4,   3,  -3,
	    -14,   2, -11,  -2,  -5,   2,  14,   5,
	    -35,  -8,  11,   2,   8,  15,  -3,   1,
	     -1, -18,  -9,  10, -15, -25, -31, -50,
	},

	// pawn
	{
	      0,   0,   0,   0,   0,   0,  0,   0,
	     98, 134,  61,  95,  68, 126, 34, -11,
	     -6,   7,  26,  31,  65,  56, 25, -20,
	    -14,  13,   6,  21,  23,  12, 17, -23,
	    -27,  -2,  -5,  12,  17,   6, 10, -25,
	    -26,  -4,  -4, -10,   3,   3, 33, -12,
	    -35,  -1, -20, -23, -15,  24, 38, -22,
	      0,   0,   0,   0,   0,   0,  0,   0,
	},

	// knight
	{
	    -167, -89, -34, -49,  61, -97, -15, -107,
	     -73, -41,  72,  36,  23,  62,   7,  -17,
	     -47,  60,  37,  65,  84, 129,  73,   44,
	      -9,  17,  19,  53,  37,  69,  18,   22,
	     -13,   4,  16,  13,  28,  19,  21,   -8,
	     -23,  -9,  12,  10,  19,  17,  25,  -16,
	     -29, -53, -12,  -3,  -1,  18, -14,  -19,
	    -105, -21, -58, -33, -17, -28, -19,  -23,
	},

	// king
	{
	    -65,  23,  16, -15, -56, -34,   2,  13,
	     29,  -1, -20,  -7,  -8,  -4, -38, -29,
	     -9,  24,   2, -16, -20,   6,  22, -22,
	    -17, -20, -12, -27, -30, -25, -14, -36,
	    -49,  -1, -27, -39, -46, -44, -33, -51,
	    -14, -14, -22, -46, -44, -30, -15, -27,
	      1,   7,  -8, -64, -43, -16,   9,   8,
	    -15,  36,  12, -54,   8, -28,  24,  14,
	}
	},
	// endgame tables
	{
	// bishop
	{
	    -14, -21, -11,  -8, -7,  -9, -17, -24,
	     -8,  -4,   7, -12, -3, -13,  -4, -14,
	      2,  -8,   0,  -1, -2,   6,   0,   4,
	     -3,   9,  12,   9, 14,  10,   3,   2,
	     -6,   3,  13,  19,  7,  10,  -3,  -9,
	    -12,  -3,   8,  10, 13,   3,  -7, -15,
	    -14, -18,  -7,  -1,  4,  -9, -15, -27,
	    -23,  -9, -23,  -5, -9, -16,  -5, -17,
	},
	// rook
	{
	    13, 10, 18, 15, 12,  12,   8,   5,
	    11, 13, 13, 11, -3,   3,   8,   3,
	     7,  7,  7,  5,  4,  -3,  -5,  -3,
	     4,  3, 13,  1,  2,   1,  -1,   2,
	     3,  5,  8,  4, -5,  -6,  -8, -11,
	    -4,  0, -5, -1, -7, -12,  -8, -16,
	    -6, -6,  0,  2, -9,  -9, -11,  -3,
	    -9,  2,  3, -1, -5, -13,   4, -20,
	},
	// queen
	{
	     -9,  22,  22,  27,  27,  19,  10,  20,
	    -17,  20,  32,  41,  58,  25,  30,   0,
	    -20,   6,   9,  49,  47,  35,  19,   9,
	      3,  22,  24,  45,  57,  40,  57,  36,
	    -18,  28,  19,  47,  31,  34,  39,  23,
	    -16, -27,  15,   6,   9,  17,  10,   5,
	    -22, -23, -30, -16, -16, -23, -36, -32,
	    -33, -28, -22, -43,  -5, -32, -20, -41,
	},
	// pawn
	{
	      0,   0,   0,   0,   0,   0,   0,   0,
	    178, 173, 158, 134, 147, 132, 165, 187,
	     94, 100,  85,  67,  56,  53,  82,  84,
	     32,  24,  13,   5,  -2,   4,  17,  17,
	     13,   9,  -3,  -7,  -7,  -8,   3,  -1,
	      4,   7,  -6,   1,   0,  -5,  -1,  -8,
	     13,   8,   8,  10,  13,   0,   2,  -7,
	      0,   0,   0,   0,   0,   0,   0,   0,
	},
	// knight
	{
	    -58, -38, -13, -28, -31, -27, -63, -99,
	    -25,  -8, -25,  -2,  -9, -25, -24, -52,
	    -24, -20,  10,   9,  -1,  -9, -19, -41,
	    -17,   3,  22,  22,  22,  11,   8, -18,
	    -18,  -6,  16,  25,  16,  17,   4, -18,
	    -23,  -3,  -1,  15,  10,  -3, -20, -22,
	    -42, -20, -10,  -5,  -2, -20, -23, -44,
	    -29, -51, -23, -15, -22, -18, -50, -64,
	},
	// king
	{
	    -74, -35, -18, -18, -11,  15,   4, -17,
	    -12,  17,  14,  17,  17,  38,  23,  11,
	     10,  17,  23,  15,  20,  45,  44,  13,
	     -8,  22,  24,  27,  26,  33,  26,   3,
	    -18,  -4,  21,  24,  27,  23,   9, -11,
	    -19,  -3,  11,  21,  23,  16,   7,  -9,
	    -27, -11,   4,  13,  14,   4,  -5, -17,
	    -53, -34, -21, -11, -28, -14, -24, -43
	}
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

int GetGamePhaseScore(POS *pos, int *game_phase)
{
	int white_score;
	int black_score;
	
	white_score = CountBits(pos->bb[B]) * material_score[OPENING][B];
	white_score += CountBits(pos->bb[R]) * material_score[OPENING][R];
	white_score += CountBits(pos->bb[Q]) * material_score[OPENING][Q];
	white_score += CountBits(pos->bb[N]) * material_score[OPENING][N];
	
	black_score = CountBits(pos->bb[b]) * material_score[OPENING][B];
	black_score += CountBits(pos->bb[r]) * material_score[OPENING][R];
	black_score += CountBits(pos->bb[q]) * material_score[OPENING][Q];
	black_score += CountBits(pos->bb[n]) * material_score[OPENING][N];

	int score = white_score + black_score;

	if (score > OPENING_PHASE_SCORE) *game_phase = OPENING;
	else if (score < ENDGAME_PHASE_SCORE) *game_phase = ENDGAME;
	else *game_phase = MIDDLE_GAME;

	return(score);
};

static inline void EvalBishop(POS *pos, CELL c, bool side, int *opening_score, int *endgame_score)
{
	int op_score = material_score[OPENING][B];
	int end_score = material_score[ENDGAME][B];
	op_score += positional_score[OPENING][B][(side) ? c : mirror_score[c]];
	end_score += positional_score[ENDGAME][B][(side) ? c : mirror_score[c]];
	
	int mobility = CountBits(GetBishopAttacks(c, pos->occ[black] | pos->occ[white]));
	op_score += (mobility-BISHOP_UNIT)*BISHOP_MOB_OP;
	end_score += (mobility-BISHOP_UNIT)*BISHOP_MOB_END;

	*endgame_score += (side) ? end_score : -end_score;
	*opening_score += (side) ? op_score : -op_score;
	
	return;
}

static inline void EvalRook(POS *pos, CELL c, bool side, int *opening_score, int *endgame_score)
{
	int op_score = material_score[OPENING][R];
	int end_score = material_score[ENDGAME][R];
	op_score += positional_score[OPENING][R][(side) ? c : mirror_score[c]];
	end_score += positional_score[ENDGAME][R][(side) ? c : mirror_score[c]];
	
	int mobility = CountBits(GetRookAttacks(c, pos->occ[black] | pos->occ[white]));
	op_score += mobility;
	end_score += mobility;

	if ((pos->bb[Our(P, side)] & column_masks[c]) == 0)
	{
		op_score += SEMIOPEN_COLUMN_BONUS;
		end_score += SEMIOPEN_COLUMN_BONUS;
	}
	if (((pos->bb[P] | pos->bb[p]) & column_masks[c]) == 0)
	{
		op_score += OPEN_COLUMN_BONUS;
		end_score += OPEN_COLUMN_BONUS;
	}
	
	if (pos->bb[Our(R, side)] & GetRookAttacks(c, pos->occ[black] | pos->occ[white]))
	{
		op_score += CONNECTED_ROOKS_BONUS;
		end_score += CONNECTED_ROOKS_BONUS;
	}
	
	*endgame_score += (side) ? end_score : -end_score;
	*opening_score += (side) ? op_score : -op_score;
	
	return;
}

static inline void EvalQueen(POS *pos, CELL c, bool side, int *opening_score, int *endgame_score)
{
	int op_score = material_score[OPENING][Q];
	int end_score = material_score[ENDGAME][Q];
	op_score += positional_score[OPENING][Q][(side) ? c : mirror_score[c]];
	end_score += positional_score[ENDGAME][Q][(side) ? c : mirror_score[c]];
	
	int mobility = CountBits(GetQueenAttacks(c, pos->occ[black] | pos->occ[white]));
	op_score += mobility;
	end_score += mobility;
	
	*endgame_score += (side) ? end_score : -end_score;
	*opening_score += (side) ? op_score : -op_score;

	return;
}

static inline void EvalPawn(POS *pos, CELL c, bool side, int *opening_score, int *endgame_score)
{
	int op_score = material_score[OPENING][P];
	int end_score = material_score[ENDGAME][P];
	op_score += positional_score[OPENING][P][(side) ? c : mirror_score[c]];
	end_score += positional_score[ENDGAME][P][(side) ? c : mirror_score[c]];
	
	int double_pawns = CountBits(column_masks[c] & pos->bb[Our(P, side)]);

	if (double_pawns > 1)
	{
		op_score += (double_pawns-1)*DPAWN_PENALTY_OP;
		end_score += (double_pawns-1)*DPAWN_PENALTY_END;
	}
	
	if ((pos->bb[Our(P, side)] & isolated_masks[c]) == 0)
	{
		op_score += ISOLATED_PAWN_OP;
		end_score += ISOLATED_PAWN_END;
	}
	
	if ((passed_masks[side][c] & pos->bb[Their(P, side)]) == 0)
	{ 
		op_score += passed_pawn_bonus[get_row[(side) ? c : mirror_score[c]]];
		end_score += passed_pawn_bonus[get_row[(side) ? c : mirror_score[c]]];
	}
	
	*endgame_score += (side) ? end_score : -end_score;
	*opening_score += (side) ? op_score : -op_score;

	return;
}

static inline void EvalKnight(POS *pos, CELL c, bool side, int *opening_score, int *endgame_score)
{
	int op_score = material_score[OPENING][N];
	int end_score = material_score[ENDGAME][N];
	op_score += positional_score[OPENING][N][(side) ? c : mirror_score[c]];
	end_score += positional_score[ENDGAME][N][(side) ? c : mirror_score[c]];
	
	int mobility = CountBits(KnightAttacks[c] & ~pos->occ[side]);
	op_score += mobility;
	end_score += mobility;

	*endgame_score += (side) ? end_score : -end_score;
	*opening_score += (side) ? op_score : -op_score;
	
	return;
}

static inline void EvalKing(POS *pos, CELL c, bool side, int *opening_score, int *endgame_score)
{
	int op_score = positional_score[OPENING][K][(side) ? c : mirror_score[c]];
	int end_score = positional_score[ENDGAME][K][(side) ? c : mirror_score[c]];
	
	if ((pos->bb[Our(P, side)] & column_masks[c]) == 0)
	{
		op_score -= SEMIOPEN_COLUMN_BONUS;
		end_score -= SEMIOPEN_COLUMN_BONUS;
	}

	if (((pos->bb[P] | pos->bb[p]) & column_masks[c]) == 0)
	{
		op_score -= OPEN_COLUMN_BONUS;
		end_score -= OPEN_COLUMN_BONUS;
	}
	
	int safety = CountBits(KingAttacks[c] & pos->occ[side]) * KING_SAFETY_BONUS;
	op_score += safety;
	end_score += safety;
	
	*endgame_score += (side) ? end_score : -end_score;
	*opening_score += (side) ? op_score : -op_score;
	return;
}

static void (*EvalPiece[6])(POS *pos, CELL c, bool side, int *op_score, int *end_score) = { EvalBishop, EvalRook, EvalQueen, EvalPawn, EvalKnight, EvalKing };

int Evaluate(POS *pos)
{
	int game_phase;
	int game_phase_score = GetGamePhaseScore(pos, &game_phase);
	
	int opening_score = 0;
	int endgame_score = 0;
	for (CELL c = 0; c < 64; c++)
	{
		if (pos->pt[c] == NO_PIECE) continue;
		EvalPiece[GetType(pos->pt[c])](pos, c, GetColor(pos->pt[c]), &opening_score, &endgame_score);
	}
	
	int score = 0;
	
	switch(game_phase)
	{
		case MIDDLE_GAME:
			score = (opening_score * game_phase_score + endgame_score*(OPENING_PHASE_SCORE-game_phase_score))/OPENING_PHASE_SCORE;
			break;
		case OPENING:
			score = opening_score;
			break;
		case ENDGAME:
			score = endgame_score;
			break;
	}
	
	score = (score * (100-fifty)) / 100;
	return((pos->side) ? score : -score);
}
