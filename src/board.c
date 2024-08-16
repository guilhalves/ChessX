#include "../include/board.h"
#include "../include/gen.h"
#include "../include/search.h"
#include "../include/utils.h"

static int char_pieces[] = {
	['P'] = P,
	['N'] = N,
	['B'] = B,
	['R'] = R,
	['Q'] = Q,
	['K'] = K,
	['p'] = p,
	['n'] = n,
	['b'] = b,
	['r'] = r,
	['q'] = q,
	['k'] = k
};

U64 piece_keys[12][64];
U64 enpassant_keys[65];
U64 castle_keys[16];
U64 side_key;

int hash_entries;

TT *hash_table = NULL;

void InitRandomKeys()
{
	for (int piece = B; piece <= k; piece++)
	{
		for (int cell = 0; cell < 64; cell++)
		{
			piece_keys[piece][cell] = GetRandomU64Number();
		}
	}

	for (int cell = 0; cell < 64; cell++)
	{
		enpassant_keys[cell] = GetRandomU64Number();
	}
	
	enpassant_keys[no_cell] = 0ULL;

	for (int index = 0; index < 16; index++)
	{
		castle_keys[index] = GetRandomU64Number();
	}
	
	side_key = GetRandomU64Number();

	return;
}

U64 GenerateHashKey(POS *pos)
{
	U64 final_key = 0ULL;
	
	for (CELL cell = 0; cell < 64; cell++)
	{
		if (pos->pt[cell] != NO_PIECE)
		{
			final_key ^= piece_keys[pos->pt[cell]][cell];
		}
	}
	
	if (pos->enp != no_cell)
	{
		final_key ^= enpassant_keys[pos->enp];
	}
	
	final_key ^= castle_keys[pos->castle];
	
	if (pos->side == black)
	{
		final_key ^= side_key;
	}

	return(final_key);
}

void ClearHashTable()
{
	TT *hash_entry;
	for (hash_entry = hash_table; hash_entry < hash_table + hash_entries; hash_entry++)
	{
		hash_entry->hash = 0;
		hash_entry->depth = 0;
		hash_entry->flag = 0;
		hash_entry->score = 0;
	}
	return;
}

void InitHashTable(int mb)
{
	int hash_size = 0x100000 * mb;
	hash_entries = hash_size / sizeof(TT);

	if (hash_table != NULL)
	{
		free(hash_table);
	}

	hash_table = (TT *)malloc(hash_entries * sizeof(TT));

	if (hash_table == NULL)
	{
		InitHashTable(mb/2);
	} else
	{
		ClearHashTable();
	}
	return;
}

void ParseFen(POS *pos, char *fen)
{
	memset(pos->bb, 0ULL, sizeof(pos->bb));
	memset(pos->occ, 0ULL, sizeof(pos->occ));
	
	repetition_index = 0;
	fifty = 0;

	pos->enp = no_cell;
	pos->castle = 0;

	PIECE pt;
	int offset;
	
	for (CELL cell = 0; cell < 64; cell++) pos->pt[cell] = NO_PIECE;

	for (CELL cell = 0; cell < 64; cell++)
	{
		if ((*fen >= 'a' && *fen <= 'z') || (*fen >= 'A' && *fen <= 'Z'))
		{
			pt = char_pieces[(int)(*fen)];
			SetBit(pos->bb[pt], cell);
			pos->pt[cell] = pt;
			fen++;
		}
		if (*fen >= '0' && *fen <= '9')
		{
			offset = *fen-'0';
			pt = NO_PIECE;
			
			for (int bb_pt = B; bb_pt <= k; bb_pt++)
			{
				if (GetBit(pos->bb[bb_pt], cell))
				{
					pt = bb_pt;
				}
			}

			if (pt == NO_PIECE) cell--;

			cell += offset;
			fen++;
		}
		if (*fen == '/') fen++;
	}

	fen++;

	(*fen == 'w') ? (pos->side = white) : (pos->side = black);

	fen += 2;

	while (*fen != ' ')
	{
		switch(*fen)
		{
			case 'K': pos->castle |= wk; break;
			case 'Q': pos->castle |= wq; break;
			case 'k': pos->castle |= bk; break;
			case 'q': pos->castle |= bq; break;
		}
		fen++;
	}

	fen++;

	int row;
	int column;

	if (*fen != '-')
	{
		column = fen[0] - 'a';
		row = 8 - (fen[1] - '0');
		pos->enp = row*8 + column;
	}

	for (pt = B; pt <= K; pt++)
	{
		pos->occ[white] |= pos->bb[pt];
	}

	for (pt = b; pt <= k; pt++)
	{
		pos->occ[black] |= pos->bb[pt];
	}
	
	pos->hash = GenerateHashKey(pos);

	return;
}
