#include "../include/bitboard.h"
#include "../include/attacks.h"
#include "../include/gen.h"

CELL CountBits(U64 bitboard)
{
	CELL count = 0;

	while (bitboard)
	{
		count++;
		bitboard &= bitboard - 1;
	}
	return(count);
}

CELL GetLsBit(U64 bitboard)
{
	return(CountBits((bitboard & -bitboard)-1));
}

CELL PopLsBit(U64 *bitboard)
{
	CELL cell = GetLsBit(*bitboard);
	PopBit(*bitboard, cell);
	return(cell);
}
