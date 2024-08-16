#ifndef UTILS_H
#define UTILS_H

#include "bitboard.h"
#include "attacks.h"
#include "board.h"
#include "gen.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef WIN64
	#include <windows.h>
#else
	#include <sys/time.h>
#endif

#define empty_board "8/8/8/8/8/8/8/8 w - - "
#define start_position "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1 "
#define tricky_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "
#define killer_position "rnbqkb1r/pp1p1pPp/8/2p1pP2/1P1P4/3P3P/P1P1P3/RNBQKBNR w KQkq e6 0 1 "
#define cmk_position "r2q1rk1/ppp2ppp/2n1bn2/2b1p3/3pP3/3P1NPP/PPP1NPB1/R1BQ1RK1 b - - 0 9 "
#define repetitions "2r3k1/R7/8/1R6/8/8/P4KPP/8 w - - 0 40 "

void PrintBitboard(U64 bitboard);

void PrintBoard(POS pos);

U64 GetRandomU64Number();

int GetTimeMs();

void PrintMove(int move);

void PrintMoveList(LIST list);

void PerftTest(POS *pos, int depth);

void InitCellsBetween();

#endif
