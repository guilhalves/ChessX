#include "../include/utils.h"

static const char *cell_to_coord[] =
{
	"a8", "b8", "c8", "d8", "e8", "f8", "g8", "h8",
	"a7", "b7", "c7", "d7", "e7", "f7", "g7", "h7",
	"a6", "b6", "c6", "d6", "e6", "f6", "g6", "h6",
	"a5", "b5", "c5", "d5", "e5", "f5", "g5", "h5",
	"a4", "b4", "c4", "d4", "e4", "f4", "g4", "h4",
	"a3", "b3", "c3", "d3", "e3", "f3", "g3", "h3",
	"a2", "b2", "c2", "d2", "e2", "f2", "g2", "h2",
	"a1", "b1", "c1", "d1", "e1", "f1", "g1", "h1"
};

#ifdef WIN64
	static char ascii_pieces[12] = "BRQPNKbrqpnk";
#else
	static char *unicode_pieces[12] = {"♗", "♖", "♕", "♙", "♘", "♔", "♝", "♜", "♛", "♟︎", "♞", "♚"};
#endif

void PrintBitboard(U64 bitboard)
{
	for (int cell = 0; cell < 64; cell++)
	{
		if (cell%8 == 0)
		{
			printf("\n");
			printf("  %d ", 8-cell/8);
		}
		printf(" %d", GetBit(bitboard, cell) ? 1 : 0);
	}
	printf("\n\n     a b c d e f g h\n\n");
	printf("     Bitboard: %llud\n\n", bitboard);
}

static unsigned int state = 1804289383;

static unsigned int GetRandomU32Number()
{
	unsigned int number = state;

	number ^= number << 13;
	number ^= number >> 17;
	number ^= number << 5;

	state = number;

	return(number);
}

U64 GetRandomU64Number()
{
	U64 n1, n2, n3, n4;

	n1 = (U64)(GetRandomU32Number()) & 0xFFFF;
	n2 = (U64)(GetRandomU32Number()) & 0xFFFF;
	n3 = (U64)(GetRandomU32Number()) & 0xFFFF;
	n4 = (U64)(GetRandomU32Number()) & 0xFFFF;

	return(n1 | (n2 << 16) | (n3 << 32) | (n4 << 48));
}

int GetTimeMs()
{
	#ifdef WIN64
		return(GetTickCount());
	#else
		struct timeval time_value;
		gettimeofday(&time_value, NULL);
		return((int)(time_value.tv_sec*1000) + (int)(time_value.tv_usec / 1000));  
	#endif
}

void PrintBoard(POS pos)
{
	int piece;

	printf("\n");
	
	for (int cell = 0; cell < 64; cell++)
	{
		
		if (cell%8 == 0)
		{
			printf("\n");
			printf("  %d ", 8-cell/8);
		}
		
		piece = pos.pt[cell];
		
		#ifdef WIN64
			printf(" %c", (piece == NO_PIECE) ? '.' : ascii_pieces[piece]);
		#else
			printf(" %s", (piece == NO_PIECE) ? "." : unicode_pieces[piece]);
		#endif
	}

	printf("\n\n     a b c d e f g h\n\n");

	printf("     Turn:         %s\n", (pos.side) ? "w" : "b");

	printf("     Enpassant:   %s\n", (pos.enp != no_cell) ? cell_to_coord[pos.enp] : "no");

	printf("     Castling:  %c%c%c%c\n", (pos.castle & wk) ? 'K' : '-', (pos.castle & wq) ? 'Q' : '-', (pos.castle & bk) ? 'k' : '-', (pos.castle & bq) ? 'q' : '-');
	printf("     Hash key: %llx\n\n", pos.hash);
	return;
}

void PrintMove(int move)
{
	printf("%s%s", cell_to_coord[GetFrom(move)], cell_to_coord[GetTo(move)]);
	switch (GetFlag(move))
	{
		case PR_KNIGHT:
		case PC_KNIGHT:
			printf("n");
			break;
		case PR_BISHOP:
		case PC_BISHOP:
			printf("b");
			break;
		case PR_ROOK:
		case PC_ROOK:
			printf("r");
			break;
		case PR_QUEEN:
		case PC_QUEEN:
			printf("q");
			break;
	}
	return;
}

void PrintMoveList(LIST list)
{
	printf("MOVE LIST\n\n");

	for (int count = 0; count < list.end; count++)
	{
		PrintMove(list.moves[count]);
		printf("\n");
	}

	printf("\n%d MOVES\n\n", list.end);
	return;
}

U64 perft_nodes;

static inline void PerftDriver(POS *pos, int depth)
{
	if (depth == 0)
	{
		perft_nodes++;
		return;
	}

	LIST list;

	GenMoves(pos, &list);
	
	POS new;

	for (int count = 0; count < list.end; count++)
	{
		memcpy(&new, pos, sizeof(POS));

		if (!MakeMove(&new, list.moves[count])) continue;

		PerftDriver(&new, depth-1);
	}

	return;
}

void PerftTest(POS *pos, int depth)
{
	printf("\n     Performance test\n\n");

	LIST list;

	GenMoves(pos, &list);

	perft_nodes = 0;

	U64 temp_nodes;

	int start = GetTimeMs();

	POS new;

	for (int count = 0; count < list.end; count++)
	{
		memcpy(&new, pos, sizeof(POS));

		if (!MakeMove(&new, list.moves[count])) continue;

		temp_nodes = perft_nodes;

		PerftDriver(&new, depth - 1);

		printf("     move : %s%s", cell_to_coord[GetFrom(list.moves[count])], cell_to_coord[GetTo(list.moves[count])]);
		switch(GetFlag(list.moves[count]))
		{
			case PR_KNIGHT:
			case PC_KNIGHT:
				printf("n");
				break;
			case PR_BISHOP:
			case PC_BISHOP:
				printf("b");
				break;
			case PR_ROOK:
			case PC_ROOK:
				printf("r");
				break;
			case PR_QUEEN:
			case PC_QUEEN:
				printf("q");
				break;
		}
		printf("  nodes: %lld\n", perft_nodes-temp_nodes);
	}

	printf("\n\n     Depth: %d\n", depth);
	printf("     Nodes: %lld\n", perft_nodes);
	printf("     Time : %d\n\n", GetTimeMs()-start);
	printf("     Mnps  : %lf\n\n", (perft_nodes/1000.0)/(GetTimeMs()-start));
	return;
}
