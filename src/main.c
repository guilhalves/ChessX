#include "../include/bitboard.h"
#include "../include/board.h"
#include "../include/attacks.h"
#include "../include/utils.h"
#include "../include/gen.h"
#include "../include/eval.h"
#include "../include/search.h"
#include "../include/uci.h"

int main(int argc, char *argv[])
{
	InitLeapersAttacks();
	InitSlidersAttacks(bishop);
	InitSlidersAttacks(rook);
	InitCellsBetween();
	InitRandomKeys();
	InitHashTable(256);
	InitEvaluationMasks();

	POS pos;
	ParseFen(&pos, empty_board);

	if (argc > 1 && !strncmp(argv[1], "gen", 3))
	{
		ParseFen(&pos, start_position);
		
		for (int i = 2; i <= 6; i++)
		{
			PerftTest(&pos, i);
		}
		ParseGo(&pos, "go depth 10");

		ParseFen(&pos, kiwi_position);
		
		for (int i = 2; i < 5; i++)
		{
			PerftTest(&pos, i);
		}
		ParseGo(&pos, "go depth 10");

		return(0);
	}

	UCILoop(&pos);

	return(0);
}
