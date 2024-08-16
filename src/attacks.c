#include "../include/attacks.h"

#define NotAColumn 18374403900871474942ULL

#define NotHColumn 9187201950435737471ULL

#define NotABColumn 18229723555195321596ULL

#define NotGHColumn 4557430888798830399ULL

U64 PawnAttacks[2][65];

U64 KnightAttacks[64];

U64 KingAttacks[64];

U64 BishopMasks[64];

U64 RookMasks[64];

U64 BishopAttacks[64][512];

U64 RookAttacks[64][4096];

const int bishop_relevant_bits[64] =
{
	6, 5, 5, 5, 5, 5, 5, 6, 
	5, 5, 5, 5, 5, 5, 5, 5, 
	5, 5, 7, 7, 7, 7, 5, 5, 
	5, 5, 7, 9, 9, 7, 5, 5, 
	5, 5, 7, 9, 9, 7, 5, 5, 
	5, 5, 7, 7, 7, 7, 5, 5, 
	5, 5, 5, 5, 5, 5, 5, 5, 
	6, 5, 5, 5, 5, 5, 5, 6
};

const int rook_relevant_bits[64] =
{
	12, 11, 11, 11, 11, 11, 11, 12, 
	11, 10, 10, 10, 10, 10, 10, 11, 
	11, 10, 10, 10, 10, 10, 10, 11, 
	11, 10, 10, 10, 10, 10, 10, 11, 
	11, 10, 10, 10, 10, 10, 10, 11, 
	11, 10, 10, 10, 10, 10, 10, 11, 
	11, 10, 10, 10, 10, 10, 10, 11, 
	12, 11, 11, 11, 11, 11, 11, 12
};

U64 bishop_magic_numbers[64] =
{
	0x40040844404084ULL,
	0x2004208a004208ULL,
	0x10190041080202ULL,
	0x108060845042010ULL,
	0x581104180800210ULL,
	0x2112080446200010ULL,
	0x1080820820060210ULL,
	0x3c0808410220200ULL,
	0x4050404440404ULL,
	0x21001420088ULL,
	0x24d0080801082102ULL,
	0x1020a0a020400ULL,
	0x40308200402ULL,
	0x4011002100800ULL,
	0x401484104104005ULL,
	0x801010402020200ULL,
	0x400210c3880100ULL,
	0x404022024108200ULL,
	0x810018200204102ULL,
	0x4002801a02003ULL,
	0x85040820080400ULL,
	0x810102c808880400ULL,
	0xe900410884800ULL,
	0x8002020480840102ULL,
	0x220200865090201ULL,
	0x2010100a02021202ULL,
	0x152048408022401ULL,
	0x20080002081110ULL,
	0x4001001021004000ULL,
	0x800040400a011002ULL,
	0xe4004081011002ULL,
	0x1c004001012080ULL,
	0x8004200962a00220ULL,
	0x8422100208500202ULL,
	0x2000402200300c08ULL,
	0x8646020080080080ULL,
	0x80020a0200100808ULL,
	0x2010004880111000ULL,
	0x623000a080011400ULL,
	0x42008c0340209202ULL,
	0x209188240001000ULL,
	0x400408a884001800ULL,
	0x110400a6080400ULL,
	0x1840060a44020800ULL,
	0x90080104000041ULL,
	0x201011000808101ULL,
	0x1a2208080504f080ULL,
	0x8012020600211212ULL,
	0x500861011240000ULL,
	0x180806108200800ULL,
	0x4000020e01040044ULL,
	0x300000261044000aULL,
	0x802241102020002ULL,
	0x20906061210001ULL,
	0x5a84841004010310ULL,
	0x4010801011c04ULL,
	0xa010109502200ULL,
	0x4a02012000ULL,
	0x500201010098b028ULL,
	0x8040002811040900ULL,
	0x28000010020204ULL,
	0x6000020202d0240ULL,
	0x8918844842082200ULL,
	0x4010011029020020ULL
};

U64 rook_magic_numbers[64] =
{
	0x8a80104000800020ULL,
	0x140002000100040ULL,
	0x2801880a0017001ULL,
	0x100081001000420ULL,
	0x200020010080420ULL,
	0x3001c0002010008ULL,
	0x8480008002000100ULL,
	0x2080088004402900ULL,
	0x800098204000ULL,
	0x2024401000200040ULL,
	0x100802000801000ULL,
	0x120800800801000ULL,
	0x208808088000400ULL,
	0x2802200800400ULL,
	0x2200800100020080ULL,
	0x801000060821100ULL,
	0x80044006422000ULL,
	0x100808020004000ULL,
	0x12108a0010204200ULL,
	0x140848010000802ULL,
	0x481828014002800ULL,
	0x8094004002004100ULL,
	0x4010040010010802ULL,
	0x20008806104ULL,
	0x100400080208000ULL,
	0x2040002120081000ULL,
	0x21200680100081ULL,
	0x20100080080080ULL,
	0x2000a00200410ULL,
	0x20080800400ULL,
	0x80088400100102ULL,
	0x80004600042881ULL,
	0x4040008040800020ULL,
	0x440003000200801ULL,
	0x4200011004500ULL,
	0x188020010100100ULL,
	0x14800401802800ULL,
	0x2080040080800200ULL,
	0x124080204001001ULL,
	0x200046502000484ULL,
	0x480400080088020ULL,
	0x1000422010034000ULL,
	0x30200100110040ULL,
	0x100021010009ULL,
	0x2002080100110004ULL,
	0x202008004008002ULL,
	0x20020004010100ULL,
	0x2048440040820001ULL,
	0x101002200408200ULL,
	0x40802000401080ULL,
	0x4008142004410100ULL,
	0x2060820c0120200ULL,
	0x1001004080100ULL,
	0x20c020080040080ULL,
	0x2935610830022400ULL,
	0x44440041009200ULL,
	0x280001040802101ULL,
	0x2100190040002085ULL,
	0x80c0084100102001ULL,
	0x4024081001000421ULL,
	0x20030a0244872ULL,
	0x12001008414402ULL,
	0x2006104900a0804ULL,
	0x1004081002402ULL
};

static U64 MaskPawnAttacks(int side, int cell)
{

	U64 attacks = 0ULL;

	U64 bitboard = 0ULL;

	SetBit(bitboard, cell);

	// white
	if (side)
	{
		if ((bitboard >> 7) & NotAColumn)
		{
			attacks |= (bitboard >> 7);
		}
		if ((bitboard >> 9) & NotHColumn)
		{
			attacks |= (bitboard >> 9);
		}
	} else // black
	{
		if ((bitboard << 7) & NotHColumn)
		{
			attacks |= (bitboard << 7);
		}
		if ((bitboard << 9) & NotAColumn)
		{
			attacks |= (bitboard << 9);
		}
	}

	return(attacks);
}

static U64 MaskKnightAttacks(int cell)
{
	U64 attacks = 0ULL;

	U64 bitboard = 0ULL;

	SetBit(bitboard, cell);

	if ((bitboard >> 6) & NotABColumn)
	{
		attacks |= (bitboard >> 6);
	}
	if ((bitboard >> 10) & NotGHColumn)
	{
		attacks |= (bitboard >> 10);
	}
	if ((bitboard >> 15) & NotAColumn)
	{
		attacks |= (bitboard >> 15);
	}
	if ((bitboard >> 17) & NotHColumn)
	{
		attacks |= (bitboard >> 17);
	}

	if ((bitboard << 6) & NotGHColumn)
	{
		attacks |= (bitboard << 6);
	}
	if ((bitboard << 10) & NotABColumn)
	{
		attacks |= (bitboard << 10);
	}
	if ((bitboard << 15) & NotHColumn)
	{
		attacks |= (bitboard << 15);
	}
	if ((bitboard << 17) & NotAColumn)
	{
		attacks |= (bitboard << 17);
	}

	return(attacks);
}

static U64 MaskKingAttacks(int cell)
{
	U64 attacks = 0ULL;

	U64 bitboard = 0ULL;

	SetBit(bitboard, cell);

	if ((bitboard >> 1) & NotHColumn)
	{
		attacks |= (bitboard >> 1);
	}
	if ((bitboard >> 7) & NotAColumn)
	{
		attacks |= (bitboard >> 7);
	}
	if ((bitboard >> 9) & NotHColumn)
	{
		attacks |= (bitboard >> 9);
	}
	if (bitboard >> 8)
	{
		attacks |= (bitboard >> 8);
	}

	if ((bitboard << 1) & NotAColumn)
	{
		attacks |= (bitboard << 1);
	}
	if ((bitboard << 7) & NotHColumn)
	{
		attacks |= (bitboard << 7);
	}
	if ((bitboard << 9) & NotAColumn)
	{
		attacks |= (bitboard << 9);
	}
	if (bitboard << 8)
	{
		attacks |= (bitboard << 8);
	}

	return(attacks);
}

static U64 MaskBishopAttacks (int cell)
{
	U64 attacks = 0ULL;

	int r, c; // row & column

	// target row & target column

	int tr = cell/8;
	int tc = cell%8;

	for (r = tr+1, c = tc+1; r <= 6 && c <= 6; r++, c++)
	{
		attacks |= (1ULL << (r*8+c));
	}

	for (r = tr-1, c = tc-1; r > 0 && c > 0; r--, c--)
	{
		attacks |= (1ULL << (r*8+c));
	}

	for (r = tr-1, c = tc+1; r > 0 && c <= 6; r--, c++)
	{
		attacks |= (1ULL << (r*8+c));
	}

	for (r = tr+1, c = tc-1; r <= 6 && c > 0; r++, c--)
	{
		attacks |= (1ULL << (r*8+c));
	}

	return(attacks);
}

static U64 MaskRookAttacks (int cell)
{
	U64 attacks = 0ULL;

	int r, c; // row & column

	// target row & target column

	int tr = cell/8;
	int tc = cell%8;

	for (r = tr+1; r <= 6; r++)
	{
		attacks |= (1ULL << (r*8+tc));
	}

	for (r = tr-1; r > 0; r--)
	{
		attacks |= (1ULL << (r*8+tc));
	}

	for (c = tc+1; c <= 6; c++)
	{
		attacks |= (1ULL << (tr*8+c));
	}

	for (c = tc-1; c > 0; c--)
	{
		attacks |= (1ULL << (tr*8+c));
	}

	return(attacks);
}

static U64 BishopAttacksOnTheFly (int cell, U64 block)
{
	U64 attacks = 0ULL;

	int r, c; // row & column

	// target row & target column

	int tr = cell/8;
	int tc = cell%8;

	for (r = tr+1, c = tc+1; r <= 7 && c <= 7; r++, c++)
	{
		attacks |= (1ULL << (r*8+c));
		if ((1ULL << (r*8+c)) & block) break;
	}

	for (r = tr-1, c = tc-1; r >= 0 && c >= 0; r--, c--)
	{
		attacks |= (1ULL << (r*8+c));
		if ((1ULL << (r*8+c)) & block) break;
	}

	for (r = tr-1, c = tc+1; r >= 0 && c <= 7; r--, c++)
	{
		attacks |= (1ULL << (r*8+c));
		if ((1ULL << (r*8+c)) & block) break;
	}

	for (r = tr+1, c = tc-1; r <= 7 && c >= 0; r++, c--)
	{
		attacks |= (1ULL << (r*8+c));
		if ((1ULL << (r*8+c)) & block) break;
	}

	return(attacks);
}

static U64 RookAttacksOnTheFly (int cell, U64 block)
{
	U64 attacks = 0ULL;

	int r, c; // row & column

	// target row & target column

	int tr = cell/8;
	int tc = cell%8;

	for (r = tr+1; r <= 7; r++)
	{
		attacks |= (1ULL << (r*8+tc));
		if ((1ULL << (r*8+tc)) & block) break;
	}

	for (r = tr-1; r >= 0; r--)
	{
		attacks |= (1ULL << (r*8+tc));
		if ((1ULL << (r*8+tc)) & block) break;
	}

	for (c = tc+1; c <= 7; c++)
	{
		attacks |= (1ULL << (tr*8+c));
		if ((1ULL << (tr*8+c)) & block) break;
	}

	for (c = tc-1; c >= 0; c--)
	{
		attacks |= (1ULL << (tr*8+c));
		if ((1ULL << (tr*8+c)) & block) break;
	}

	return(attacks);
}

static U64 SetOccupancy(int index, int bits_in_mask, U64 attack_mask)
{
	U64 occupancy = 0ULL;

	int cell;

	for (int count = 0; count < bits_in_mask; count++)
	{
		cell = GetLsBit(attack_mask);

		PopBit(attack_mask, cell);

		if (index & (1 << count))
		{
			occupancy |= (1ULL << cell);
		}
	}

	return(occupancy);
}

void InitLeapersAttacks()
{
	for (int cell = 0; cell < 64; cell++)
	{
		PawnAttacks[white][cell] = MaskPawnAttacks(white, cell);
		PawnAttacks[black][cell] = MaskPawnAttacks(black, cell);
		KnightAttacks[cell] = MaskKnightAttacks(cell);
		KingAttacks[cell] = MaskKingAttacks(cell);
	}

	PawnAttacks[black][no_cell] = 0;
	PawnAttacks[white][no_cell] = 0;
	
	return;
}

void InitSlidersAttacks(int bishop)
{
	U64 attack_mask;

	int occupancy_indices;

	U64 occupancy;

	int relevant_bits_count;

	int magic_index;

	for (int cell = 0; cell < 64; cell++)
	{
		BishopMasks[cell] = MaskBishopAttacks(cell);
		RookMasks[cell] = MaskRookAttacks(cell);

		attack_mask = bishop ? BishopMasks[cell] : RookMasks[cell];

		relevant_bits_count = CountBits(attack_mask);

		occupancy_indices = (1 << relevant_bits_count);

		for (int index = 0; index < occupancy_indices; index++)
		{
			if (bishop)
			{
				occupancy = SetOccupancy(index, relevant_bits_count, attack_mask);
				magic_index = (occupancy * bishop_magic_numbers[cell]) >> (64-bishop_relevant_bits[cell]);
				BishopAttacks[cell][magic_index] = BishopAttacksOnTheFly(cell, occupancy);
			} else
			{
				occupancy = SetOccupancy(index, relevant_bits_count, attack_mask);
				magic_index = (occupancy * rook_magic_numbers[cell]) >> (64-rook_relevant_bits[cell]);
				RookAttacks[cell][magic_index] = RookAttacksOnTheFly(cell, occupancy);
			}
		}
	}

	return;
}
