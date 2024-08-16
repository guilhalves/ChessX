#ifndef ATTACK_H
#define ATTACK_H

#include "bitboard.h"

// Inicializa máscaras

void InitLeapersAttacks();

enum { rook, bishop };

void InitSlidersAttacks(int bishop);

// Variáveis globais

extern U64 PawnAttacks[2][65];

extern U64 KnightAttacks[64];

extern U64 KingAttacks[64];

extern U64 BishopMasks[64];

extern U64 RookMasks[64];

extern U64 BishopAttacks[64][512];

extern U64 RookAttacks[64][4096];

extern const int bishop_relevant_bits[64];

extern const int rook_relevant_bits[64];

extern U64 bishop_magic_numbers[64];

extern U64 rook_magic_numbers[64];

#endif
