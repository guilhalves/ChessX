#include "../include/gen.h"
#include "../include/search.h"

#define ShiftRelN(bb, c) ((c) ? bb >> 8 : bb << 8)
#define RelOffsetN(cell, c) ((c) ? (cell) - 8 : (cell) + 8)
#define RelOffsetS(cell, c) ((c) ? (cell) + 8 : (cell) - 8)
#define RelOffsetNN(cell, c) ((c) ? (cell) - 16 : (cell) + 16)
#define RelOffsetSS(cell, c) ((c) ? (cell) + 16 : (cell) - 16)
#define BackRank(c) ((c) ? 0xff00000000000000 : 0xff)
#define PromRank(c) ((c) ? 0xff00 : 0xff000000000000)

U64 cells_between[64][64];

const U64 cell_bb[65] =
{
	0x1, 0x2, 0x4, 0x8,
	0x10, 0x20, 0x40, 0x80,
	0x100, 0x200, 0x400, 0x800,
	0x1000, 0x2000, 0x4000, 0x8000,
	0x10000, 0x20000, 0x40000, 0x80000,
	0x100000, 0x200000, 0x400000, 0x800000,
	0x1000000, 0x2000000, 0x4000000, 0x8000000,
	0x10000000, 0x20000000, 0x40000000, 0x80000000,
	0x100000000, 0x200000000, 0x400000000, 0x800000000,
	0x1000000000, 0x2000000000, 0x4000000000, 0x8000000000,
	0x10000000000, 0x20000000000, 0x40000000000, 0x80000000000,
	0x100000000000, 0x200000000000, 0x400000000000, 0x800000000000,
	0x1000000000000, 0x2000000000000, 0x4000000000000, 0x8000000000000,
	0x10000000000000, 0x20000000000000, 0x40000000000000, 0x80000000000000,
	0x100000000000000, 0x200000000000000, 0x400000000000000, 0x800000000000000,
	0x1000000000000000, 0x2000000000000000, 0x4000000000000000, 0x8000000000000000,
	0x0
};

static const int castling_rights[64] =
{
	 7, 15, 15, 15,  3, 15, 15, 11,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	15, 15, 15, 15, 15, 15, 15, 15,
	13, 15, 15, 15, 12, 15, 15, 14
};

U64 GetBishopAttacks(CELL cell, U64 occupancy)
{
	return(BishopAttacks[cell][(((occupancy & BishopMasks[cell])*bishop_magic_numbers[cell]) >> (64-bishop_relevant_bits[cell]))]);
}

U64 GetRookAttacks(CELL cell, U64 occupancy)
{
	return(RookAttacks[cell][(((occupancy & RookMasks[cell])*rook_magic_numbers[cell]) >> (64-rook_relevant_bits[cell]))]);
}

U64 GetQueenAttacks(CELL cell, U64 occupancy)
{
	return(GetBishopAttacks(cell, occupancy) | GetRookAttacks(cell, occupancy));
}

void InitCellsBetween()
{
	for (CELL cell1 = a8; cell1 <= h1; ++cell1)
	{
		for (CELL cell2 = a8; cell2 <= h1; ++cell2)
		{
			if (cell1%8 == cell2%8 || cell1/8 == cell2/8)
			{
				cells_between[cell1][cell2] =
				(GetRookAttacks(cell1, 0) & GetRookAttacks(cell2, 0)) | cell_bb[cell1] | cell_bb[cell2];
			} else if (7+cell1/8-cell1%8 == 7+cell2/8-cell2%8 || cell1/8 + cell1%8 == cell2/8 + cell2%8)
			{
				cells_between[cell1][cell2] =
				(GetBishopAttacks(cell1, 0) & GetBishopAttacks(cell2, 0)) | cell_bb[cell1] | cell_bb[cell2];
			}
		}
	}
}

static inline int EncodeMove(CELL from, CELL to, short int flag)
{
	return((flag << 12) | (from << 6) | to);
}

static U64 (*SlidingAttacks[3])(CELL cell, U64 occupancy) = { GetBishopAttacks, GetRookAttacks, GetQueenAttacks };

static inline void GetMoves(CELL from, U64 to, short int flag, LIST *list)
{
	while (to)
	{
		list->moves[list->end] = EncodeMove(from, PopLsBit(&to), flag);
		list->end = list->end + 1;
	}
	return;
}

static inline void GetSlidingAttacks(PIECE pt, U64 occ, U64 bb, U64 mask, short int flag, LIST *list)
{
	CELL c;
	while (bb)
	{
		c = PopLsBit(&bb);
		GetMoves(c, SlidingAttacks[GetType(pt)](c, occ) & mask, flag, list);
	}
	return;
}

static inline void GetLeapersAttacks(PIECE pt, U64 bb, U64 mask, short int flag, LIST *list)
{
	CELL c;
	
	switch (GetType(pt))
	{
		case P:
			while (bb) { c = PopLsBit(&bb); GetMoves(c, PawnAttacks[GetColor(pt)][c] & mask, flag, list); }
			break;
		case N:
			while (bb) { c = PopLsBit(&bb); GetMoves(c, KnightAttacks[c] & mask, flag, list); }
			break;
		case K:
			while (bb) { c = PopLsBit(&bb); GetMoves(c, KingAttacks[c] & mask, flag, list); }
			break;
	}
}

static inline U64 GetAttackedCells(POS *pos)
{
	U64 attacked = KingAttacks[GetLsBit(pos->bb[Their(K, pos->side)])];
	U64 occ = (pos->occ[white] | pos->occ[black]) & ~pos->bb[Our(K, pos->side)];

	U64 b1 = pos->bb[Their(P, pos->side)];
	while (b1) attacked |= PawnAttacks[pos->side^1][PopLsBit(&b1)];
	
	b1 = pos->bb[Their(N, pos->side)];
	while (b1) attacked |= KnightAttacks[PopLsBit(&b1)];
	
	b1 = pos->bb[Their(B, pos->side)] | pos->bb[Their(Q, pos->side)];
	while (b1) attacked |= GetBishopAttacks(PopLsBit(&b1), occ);
	
	b1 = pos->bb[Their(R, pos->side)] | pos->bb[Their(Q, pos->side)];
	while (b1) attacked |= GetRookAttacks(PopLsBit(&b1), occ);
	
	return(attacked);
}

static inline U64 GetCheckingPieces(POS *pos, U64 them)
{
	CELL our_king = GetLsBit(pos->bb[Our(K, pos->side)]);

	U64 checking = (KnightAttacks[our_king] & pos->bb[Their(N, pos->side)]) | (PawnAttacks[pos->side][our_king] & pos->bb[Their(P, pos->side)]);

	U64 candidates = (GetBishopAttacks(our_king, them) & (pos->bb[Their(B, pos->side)] | pos->bb[Their(Q, pos->side)])) | (GetRookAttacks(our_king, them) & (pos->bb[Their(R, pos->side)] | pos->bb[Their(Q, pos->side)]));

	U64 bb;
	U64 mask = (pos->occ[white] | pos->occ[black]) & ~them;
	CELL c;

	while (candidates)
	{
		c = PopLsBit(&candidates);
		bb = cells_between[our_king][c] & mask;
		if (bb == 0) checking |= cell_bb[c];
	}
	return(checking);
}

void GenMoves(POS *pos, LIST *list)
{
	list->end = 0;
	
	U64 us, them, all, move_mask, attacked, checking, b1;
	us = pos->occ[pos->side];
	them = pos->occ[pos->side^1];
	all = pos->occ[white] | pos->occ[black];
	
	attacked = GetAttackedCells(pos);
	
	CELL our_king = GetLsBit(pos->bb[Our(K, pos->side)]);
	
	b1 = KingAttacks[our_king] & ~(us | attacked);
	
	// KING MOVES
	GetLeapersAttacks(Our(K, pos->side), cell_bb[our_king], b1 & ~them, QUIET, list);
	GetLeapersAttacks(Our(K, pos->side), cell_bb[our_king], b1 & them, CAPTURE, list);
	
	checking = GetCheckingPieces(pos, them);
	
	int num_checkers = CountBits(checking);
	
	switch(num_checkers)
	{
		case 2:
			return;
		case 1:
			move_mask = checking | cells_between[GetLsBit(checking)][our_king];
			break;
		default:
			move_mask = ~us;
			break;
	}
	
	// BISHOP MOVES
	
	GetSlidingAttacks(B, all, pos->bb[Our(B, pos->side)], move_mask & ~them, QUIET, list);
	GetSlidingAttacks(B, all, pos->bb[Our(B, pos->side)], move_mask & them, CAPTURE, list);
	
	// ROOK MOVES
	GetSlidingAttacks(R, all, pos->bb[Our(R, pos->side)], move_mask & ~them, QUIET, list);
	GetSlidingAttacks(R, all, pos->bb[Our(R, pos->side)], move_mask & them, CAPTURE, list);
	
	// QUEEN MOVES
	GetSlidingAttacks(Q, all, pos->bb[Our(Q, pos->side)], move_mask & ~them, QUIET, list);
	GetSlidingAttacks(Q, all, pos->bb[Our(Q, pos->side)], move_mask & them, CAPTURE, list);
	
	// KNIGHT MOVES
	GetLeapersAttacks(Our(N, pos->side), pos->bb[Our(N, pos->side)], move_mask & ~them, QUIET, list);
	GetLeapersAttacks(Our(N, pos->side), pos->bb[Our(N, pos->side)], move_mask & them, CAPTURE, list);
	
	// PAWN ATTACKS
	GetLeapersAttacks(Our(P, pos->side), pos->bb[Our(P, pos->side)], move_mask & them & ~BackRank(pos->side^1), CAPTURE, list);
	
	// PAWN SINGLE PUSHES

	b1 = pos->bb[Our(P, pos->side)];
	b1 = ShiftRelN(b1, pos->side);
	b1 &= ~(all | BackRank(pos->side^1));
	
	CELL c;

	while (b1)
	{
		c = PopLsBit(&b1);
		list->moves[list->end] = EncodeMove(RelOffsetS(c, pos->side), c, QUIET);
		list->end = list->end + 1;
	}
	
	// PAWN DOUBLE PUSHES
	
	b1 = pos->bb[Our(P, pos->side)] & PromRank((pos->side^1));
	b1 = ShiftRelN(b1, pos->side);
	b1 &= ~all;
	b1 = ShiftRelN(b1, pos->side);
	b1 &= ~all;

	while (b1)
	{
		c = PopLsBit(&b1);
		list->moves[list->end] = EncodeMove(RelOffsetSS(c, pos->side), c, DOUBLE_PUSH);
		list->end = list->end + 1;
	}

	// PROMOTIONS
	
	b1 = pos->bb[Our(P, pos->side)] & PromRank(pos->side);
	U64 b2 = ShiftRelN(b1, pos->side) & move_mask & ~them;

	while (b2)
	{
		c = PopLsBit(&b2);

		list->moves[list->end] = EncodeMove(RelOffsetS(c, pos->side), c, PR_QUEEN);
		list->end = list->end + 1;
		list->moves[list->end] = EncodeMove(RelOffsetS(c, pos->side), c, PR_KNIGHT);
		list->end = list->end + 1;
		list->moves[list->end] = EncodeMove(RelOffsetS(c, pos->side), c, PR_BISHOP);
		list->end = list->end + 1;
		list->moves[list->end] = EncodeMove(RelOffsetS(c, pos->side), c, PR_ROOK);
		list->end = list->end + 1;
	}

	while (b1)
	{
		c = PopLsBit(&b1);
		GetMoves(c, PawnAttacks[pos->side][c] & move_mask & them, PC_QUEEN, list);
		GetMoves(c, PawnAttacks[pos->side][c] & move_mask & them, PC_KNIGHT, list);
		GetMoves(c, PawnAttacks[pos->side][c] & move_mask & them, PC_BISHOP, list);
		GetMoves(c, PawnAttacks[pos->side][c] & move_mask & them, PC_ROOK, list);
	}

	// CASTLE
	
	if (pos->side)
	{
		if (pos->castle & wk)
		{
			if ((all & 0x6000000000000000) == 0 && (attacked & 0x7000000000000000) == 0)
			{
				list->moves[list->end] = EncodeMove(e1, g1, OO);
				list->end = list->end + 1;		
			}
		}
		if (pos->castle & wq)
		{
			if ((all & 0xe00000000000000) == 0 && (attacked & 0x1c00000000000000) == 0)
			{
				list->moves[list->end] = EncodeMove(e1, c1, OOO);
				list->end = list->end + 1;		
			}
		}
	} else
	{
		if (pos->castle & bk)
		{
			if ((all & 0x60) == 0 && (attacked & 0x70) == 0)
			{
				list->moves[list->end] = EncodeMove(e8, g8, OO);
				list->end = list->end + 1;		
			}
		}
		if (pos->castle & bq)
		{
			if ((all & 0xe) == 0 && (attacked & 0x1c) == 0)
			{
				list->moves[list->end] = EncodeMove(e8, c8, OOO);
				list->end = list->end + 1;		
			}
		}
	}

	// EN PASSANT

	b1 = PawnAttacks[pos->side^1][pos->enp] & pos->bb[Our(P, pos->side)];
	
	while (b1)
	{
		list->moves[list->end] = EncodeMove(PopLsBit(&b1), pos->enp, EN_PASSANT);
		list->end = list->end + 1;		
	}
	
	return;
}

void GenMovesQSC(POS *pos, LIST *list)
{
	list->end = 0;
	
	U64 us, them, all, move_mask, attacked, checking, b1;
	us = pos->occ[pos->side];
	them = pos->occ[pos->side^1];
	all = pos->occ[white] | pos->occ[black];
	
	attacked = GetAttackedCells(pos);
	
	CELL our_king = GetLsBit(pos->bb[Our(K, pos->side)]);
	
	b1 = KingAttacks[our_king] & ~(us | attacked) & them;
	
	// KING MOVES
	GetLeapersAttacks(Our(K, pos->side), cell_bb[our_king], b1, CAPTURE, list);
	
	checking = GetCheckingPieces(pos, them);
	
	int num_checkers = CountBits(checking);
	
	switch(num_checkers)
	{
		case 2:
			return;
		case 1:
			move_mask = checking | cells_between[GetLsBit(checking)][our_king];
			move_mask &= them;
			break;
		default:
			move_mask = ~us & them;
			break;
	}
	
	// BISHOP MOVES
	
	GetSlidingAttacks(B, all, pos->bb[Our(B, pos->side)], move_mask, CAPTURE, list);
	
	// ROOK MOVES
	GetSlidingAttacks(R, all, pos->bb[Our(R, pos->side)], move_mask, CAPTURE, list);
	
	// QUEEN MOVES
	GetSlidingAttacks(Q, all, pos->bb[Our(Q, pos->side)], move_mask, CAPTURE, list);
	
	// KNIGHT MOVES
	GetLeapersAttacks(Our(N, pos->side), pos->bb[Our(N, pos->side)], move_mask, CAPTURE, list);
	
	// PAWN ATTACKS
	GetLeapersAttacks(Our(P, pos->side), pos->bb[Our(P, pos->side)], move_mask & ~BackRank(pos->side^1), CAPTURE, list);
	
	// PROMOTIONS
	
	b1 = pos->bb[Our(P, pos->side)] & PromRank(pos->side);

	while (b1)
	{
		CELL c = PopLsBit(&b1);
		GetMoves(c, PawnAttacks[pos->side][c] & move_mask, PC_QUEEN, list);
		GetMoves(c, PawnAttacks[pos->side][c] & move_mask, PC_KNIGHT, list);
		GetMoves(c, PawnAttacks[pos->side][c] & move_mask, PC_BISHOP, list);
		GetMoves(c, PawnAttacks[pos->side][c] & move_mask, PC_ROOK, list);
	}

	// EN PASSANT

	b1 = PawnAttacks[pos->side^1][pos->enp] & pos->bb[Our(P, pos->side)];
	
	while (b1)
	{
		list->moves[list->end] = EncodeMove(PopLsBit(&b1), pos->enp, EN_PASSANT);
		list->end = list->end + 1;		
	}
	
	return;
}


int IsCellAttacked(POS *pos, CELL cell, int attacker_side)
{
	if (KnightAttacks[cell] & pos->bb[Our(N, attacker_side)]) return(1);
	if (GetBishopAttacks(cell, (pos->occ[white] | pos->occ[black])) & (pos->bb[Our(B, attacker_side)] | pos->bb[Our(Q, attacker_side)])) return(1);
	if (GetRookAttacks(cell, (pos->occ[white] | pos->occ[black])) & (pos->bb[Our(R, attacker_side)] | pos->bb[Our(Q, attacker_side)])) return(1);
	
	if (PawnAttacks[attacker_side^1][cell] & pos->bb[Our(P, attacker_side)]) return(1);
	if (KingAttacks[cell] & pos->bb[Our(K, attacker_side)]) return(1);

	return(0);
}

static inline void PutPiece(POS *pos, PIECE pc, CELL c)
{
	pos->pt[c] = pc;
	pos->bb[pc] |= cell_bb[c];
	pos->occ[GetColor(pc)] |= cell_bb[c];
	
	pos->hash ^= piece_keys[pc][c];
	
	return;
}

static inline void RemovePiece(POS *pos, CELL c)
{
	pos->bb[pos->pt[c]] &= ~cell_bb[c];
	pos->occ[GetColor(pos->pt[c])] &= ~cell_bb[c];
	
	pos->hash ^= piece_keys[pos->pt[c]][c];
	
	pos->pt[c] = NO_PIECE;
	return;
}

static inline void MovePiece(POS *pos, CELL from, CELL to)
{
	U64 mask = cell_bb[from] | cell_bb[to];
	pos->bb[pos->pt[from]] ^= mask;
	pos->bb[pos->pt[to]] &= ~mask;

	pos->occ[GetColor(pos->pt[from])] ^= mask;
	pos->occ[GetColor(pos->pt[to])] &= ~mask;
	
	pos->hash ^= piece_keys[pos->pt[from]][from];
	pos->hash ^= piece_keys[pos->pt[from]][to];
	pos->hash ^= piece_keys[pos->pt[to]][to];

	pos->pt[to] = pos->pt[from];
	pos->pt[from] = NO_PIECE;
	return;
}

static inline void MovePieceQuiet(POS *pos, CELL from, CELL to)
{
	U64 mask = cell_bb[from] | cell_bb[to];

	pos->bb[pos->pt[from]] ^= mask;
	pos->occ[GetColor(pos->pt[from])] ^= mask;
	
	pos->hash ^= piece_keys[pos->pt[from]][from];
	pos->hash ^= piece_keys[pos->pt[from]][to];
	
	pos->pt[to] = pos->pt[from];
	pos->pt[from] = NO_PIECE;
	
	return;
}

int MakeMove(POS *pos, int move)
{
	short int flag = GetFlag(move);
	CELL from = GetFrom(move);
	CELL to = GetTo(move);
	
	CELL enp_cp = no_cell;
	
	pos->hash ^= enpassant_keys[pos->enp];
	fifty++;

	switch (flag)
	{
		case QUIET:
			if (GetType(pos->pt[from]) == P) fifty = 0;
			MovePieceQuiet(pos, from, to);
			break;
		case DOUBLE_PUSH:
			MovePieceQuiet(pos, from, to);
			enp_cp = RelOffsetN(from, pos->side);
			pos->hash ^= enpassant_keys[enp_cp];
			fifty = 0;
			break;
		case OO:
			if (pos->side)
			{
				MovePieceQuiet(pos, e1, g1);
				MovePieceQuiet(pos, h1, f1);
			} else
			{
				MovePieceQuiet(pos, e8, g8);
				MovePieceQuiet(pos, h8, f8);
			}
			break;
		case OOO:
			if (pos->side)
			{
				MovePieceQuiet(pos, e1, c1);
				MovePieceQuiet(pos, a1, d1);
			} else
			{
				MovePieceQuiet(pos, e8, c8);
				MovePieceQuiet(pos, a8, d8);
			}
			break;
		case EN_PASSANT:
			MovePieceQuiet(pos, from, to);
			RemovePiece(pos, RelOffsetS(pos->enp, pos->side));
			fifty = 0;
			break;
		case PR_KNIGHT:
			RemovePiece(pos, from);
			PutPiece(pos, Our(N, pos->side), to);
			break;
		case PR_BISHOP:
			RemovePiece(pos, from);
			PutPiece(pos, Our(B, pos->side), to);
			break;
		case PR_ROOK:
			RemovePiece(pos, from);
			PutPiece(pos, Our(R, pos->side), to);
			break;
		case PR_QUEEN:
			RemovePiece(pos, from);
			PutPiece(pos, Our(Q, pos->side), to);
			break;
		case PC_KNIGHT:
			RemovePiece(pos, from);
			RemovePiece(pos, to);
			PutPiece(pos, Our(N, pos->side), to);
			fifty = 0;
			break;
		case PC_BISHOP:
			RemovePiece(pos, from);
			RemovePiece(pos, to);
			PutPiece(pos, Our(B, pos->side), to);
			fifty = 0;
			break;
		case PC_ROOK:
			RemovePiece(pos, from);
			RemovePiece(pos, to);
			PutPiece(pos, Our(R, pos->side), to);
			fifty = 0;
			break;
		case PC_QUEEN:
			RemovePiece(pos, from);
			RemovePiece(pos, to);
			PutPiece(pos, Our(Q, pos->side), to);
			fifty = 0;
			break;
		case CAPTURE:
			MovePiece(pos, from, to);
			fifty = 0;
			break;
	}
	
	if (IsCellAttacked(pos, GetLsBit(pos->bb[Our(K, pos->side)]), pos->side^1))
	{
		return(0);
	}

	pos->enp = enp_cp;
	
	pos->hash ^= castle_keys[pos->castle];

	pos->castle &= castling_rights[from];
	pos->castle &= castling_rights[to];
	
	pos->hash ^= castle_keys[pos->castle];
	
	pos->side ^= 1;
	
	pos->hash ^= side_key;

	return(1);
}
