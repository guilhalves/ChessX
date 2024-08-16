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
#define kiwi_position "r3k2r/p1ppqpb1/bn2pnp1/3PN3/1p2P3/2N2Q1p/PPPBBPPP/R3K2R w KQkq - 0 1 "

void PrintBitboard(U64 bitboard);

void PrintBoard(POS pos);

U64 GetRandomU64Number();

int GetTimeMs();

void PrintMove(int move);

void PrintMoveList(LIST list);

void PerftTest(POS *pos, int depth);

void InitCellsBetween();

#endif
