#include "../include/search.h"
#include "../include/utils.h"
#include "../include/uci.h"

#define Swap(x, y) ((x ^= y), (y ^= x), (x ^= y))

#define INFINITY 50000
#define MATE_VALUE 49000
#define MATE_SCORE 48000
#define REDUCTION_LIMIT 3
#define FULL_DEPTH_MOVES 4

static int MVV_LVA[13][13] =
{
	{303, 403, 503, 103, 203, 603,	303, 403, 503, 103, 203, 603, 0},
	{302, 402, 502, 102, 202, 602,	302, 402, 502, 102, 202, 602, 0},
	{301, 401, 501, 101, 201, 601,	301, 401, 501, 101, 201, 601, 0},
	{305, 405, 505, 105, 205, 605,	305, 405, 505, 105, 205, 605, 0},
	{304, 404, 503, 104, 204, 604,	304, 404, 503, 104, 204, 604, 0},
	{300, 400, 500, 100, 200, 600,	300, 400, 500, 100, 200, 600, 0},
	
	{303, 403, 503, 103, 203, 603,	303, 403, 503, 103, 203, 603, 0},
	{302, 402, 502, 102, 202, 602,	302, 402, 502, 102, 202, 602, 0},
	{301, 401, 501, 101, 201, 601,	301, 401, 501, 101, 201, 601, 0},
	{305, 405, 505, 105, 205, 605,	305, 405, 505, 105, 205, 605, 0},
	{304, 404, 503, 104, 204, 604,	304, 404, 503, 104, 204, 604, 0},
	{300, 400, 500, 100, 200, 600,	300, 400, 500, 100, 200, 600, 0},
	{  0,   0,   0,   0,   0,   0,    0,   0,   0,   0,   0,   0, 0}
};

U64 repetition_table[1000];

int repetition_index;

int ply;

int fifty;

U64 nodes;

LINE pv_line;

int killer_moves[2][MAX_PLY];

static inline int IsRepetition(POS *pos)
{
	for (int index = 0; index < repetition_index; index++)
	{
		if (repetition_table[index] == pos->hash) return(1);
	}
	return(0);
}

static inline int ReadHash(POS *pos, int alpha, int beta, int *bestmove, int depth)
{
	TT *hash_entry = &hash_table[pos->hash % hash_entries];
	if (hash_entry->hash == pos->hash)
	{
		if (hash_entry->depth >= depth)
		{
			int score = hash_entry->score;
			if (score < -MATE_SCORE) score += ply;
			if (score > MATE_SCORE) score -= ply;

			if (hash_entry->flag == HASH_FLAG_EXACT) return(score);
			if (hash_entry->flag == HASH_FLAG_ALPHA && score <= alpha) return(alpha);
			if (hash_entry->flag == HASH_FLAG_BETA && score >= alpha) return(beta);
		}

		*bestmove = hash_entry->bestmove;
	}
	return(NO_HASH_ENTRY);
}

static inline void WriteHash(POS *pos, int score, int bestmove, int depth, int hash_flag)
{
	TT *hash_entry = &hash_table[pos->hash % hash_entries];

	if (score < -MATE_SCORE) score -= ply;
	if (score > MATE_SCORE) score += ply;

	hash_entry->hash = pos->hash;
	hash_entry->depth = depth;
	hash_entry->flag = hash_flag;
	hash_entry->score = score;
	hash_entry->bestmove = bestmove;
}

static inline int ScoreMove(POS *pos, int move)
{
	int from = GetFrom(move);
	int to = GetTo(move);
	
	
	if (killer_moves[0][ply] == move) return(9000);
	else if (killer_moves[1][ply] == move) return(8000);
	return(MVV_LVA[pos->pt[from]][pos->pt[to]]+10000);
}

static inline void SortMoves(POS *pos, LIST *list, int bestmove)
{
	int move_scores[list->end];

	for (int count = 0; count < list->end; count++)
	{
		if (bestmove == list->moves[count]) move_scores[count] = 30000;
		else move_scores[count] = ScoreMove(pos, list->moves[count]);

	}
	
	for (int current = 0; current < list->end; current++)
	{
		for (int next = current; next < list->end; next++)
		{
			if (move_scores[current] < move_scores[next])
			{
				Swap(move_scores[current], move_scores[next]);
				Swap(list->moves[current], list->moves[next]);
			}
		}
	}
	
	return;
}

static inline int Quiescence(POS *pos, int alpha, int beta)
{
	if ((nodes & 2047) == 0) Communicate();

	nodes++;
	
	if (ply >= MAX_PLY) { return(Evaluate(pos)); }

	int eval = Evaluate(pos);
	
	if (eval >= beta) return(beta);
	
	if (eval > alpha) alpha = eval;

	LIST list;

	GenMovesQSC(pos, &list);
	
	SortMoves(pos, &list, 0);

	POS new;

	int score;
	
	for (int count = 0; count < list.end; count++)
	{
		memcpy(&new, pos, sizeof(POS));
		
		if (!MakeMove(&new, list.moves[count])) continue;
		
		ply++;

		score = -Quiescence(&new, -beta, -alpha);

		ply--;
		
		if (stopped) return(0);
		
		if (score > alpha)
		{
			alpha = score;

			if (score >= beta) return(beta);
		}
	}
	
	return(alpha);
}

static inline void MakeNullMove(POS *pos)
{
	pos->side ^= 1;
	ply++;
	repetition_index++;
	repetition_table[repetition_index] = pos->hash;
	if (pos->enp != no_cell) pos->hash ^= enpassant_keys[pos->enp];
	pos->enp = no_cell;
	pos->hash ^= side_key;
	return;
}

static inline void UndoNullMove()
{
	ply--;
	repetition_index--;
	return;
}

static inline int Contempt(POS *pos)
{
	int game_phase;
	GetGamePhaseScore(pos, &game_phase);
	
	switch(game_phase)
	{
		case MIDDLE_GAME:
			return(-25);
		case OPENING:
			return(-50);
		default:
			return(0);
	}
}

static inline int NegaMax(POS *pos, int alpha, int beta, int depth, LINE *pline)
{
	LINE line;
	line.length = 0;
	
	if ((ply && IsRepetition(pos)) || fifty >= 100) return(Contempt(pos));
	
	int score;

	int pv_node = beta-alpha > 1;
	
	if (ply && (score = ReadHash(pos, alpha, beta, pline->pv, depth)) != NO_HASH_ENTRY && pv_node == 0)
	{
		return(score);
	}

	if (depth == 0) return(Quiescence(pos, alpha, beta));
	
	if (ply >= MAX_PLY) return(Evaluate(pos));
	
	if ((nodes & 2047) == 0) Communicate();
	
	nodes++;
	
	int in_check = IsCellAttacked(pos, GetLsBit(pos->bb[Our(K, pos->side)]), pos->side^1);
	
	int f_prune = 0;

	POS new;
	
	if (in_check) depth++;
	else if (!pv_node)
	{
		score = Evaluate(pos);
		
		// static null move pruning
		if (depth < 3 && abs(beta-1) > -MATE_SCORE)
		{
			int eval_margin = 120*depth;
			if (score - eval_margin >= beta) return(score-eval_margin);
		}
		
		if (depth <= 3)
		{
			// razoring
			
			if (score+125 < beta)
			{
				int new_score = Quiescence(pos, alpha, beta);
				
				if (new_score < beta) return(new_score);
			}	
			
			// futility pruning
				
			if (abs(alpha) < MATE_SCORE && score + 80*depth <= alpha)
			{ f_prune = 1;}
			
			// reverse futility pruning

			if (score - 80*depth >= beta) return(beta);
		}

		// null move pruning
		
		if (depth >= 2 && score >= beta)
		{
			int reduction = (depth) / 4 + 3 + (((score-beta)/80 > 3) ? 3 : (score-beta)/80);

			memcpy(&new, pos, sizeof(POS));

			MakeNullMove(&new);

			score = -NegaMax(&new, -beta, -beta+1, depth - reduction, &line);

			UndoNullMove();

			if (stopped) return(0);

			if (score >= beta)
			{
				if (abs(score) >= MATE_SCORE-ply) return(beta);
				return(score);
			}
		}
	}
	
	LIST list;

	GenMoves(pos, &list);
	
	SortMoves(pos, &list, pline->pv[0]);

	int hash_flag = HASH_FLAG_ALPHA;

	int legals = 0;

	for (int count = 0; count < list.end; count++)
	{
		memcpy(&new, pos, sizeof(POS));
		
		repetition_index++;
		repetition_table[repetition_index] = pos->hash;
		
		if (!MakeMove(&new, list.moves[count])) { repetition_index--; fifty--; continue; }
		
		if 
		(
		f_prune &&
		legals &&
		(GetFlag(list.moves[count]) & CAPTURE) == 0 &&
		(GetFlag(list.moves[count]) & PROMOTION) == 0 &&
		!IsCellAttacked(pos, GetLsBit(pos->bb[Their(K, pos->side)]), pos->side)
		) { repetition_index--; fifty--; continue; }
		
		ply++;
		
		if (legals == 0) score = -NegaMax(&new, -beta, -alpha, depth - 1, &line);
		else
		{
			if
			(

			// Late move reduction conditions
			legals >= FULL_DEPTH_MOVES &&
			depth >= REDUCTION_LIMIT &&
			!in_check &&
			(GetFlag(list.moves[count]) & CAPTURE) == 0 &&
			(GetFlag(list.moves[count]) & PROMOTION) == 0
			
			) score = -NegaMax(&new, -alpha-1, -alpha, depth-2, &line);
			else score = alpha+1;
			
			if (score > alpha)
			{
				score = -NegaMax(&new, -alpha - 1, -alpha, depth - 1, &line);
				if ((score > alpha) && (score < beta)) score = -NegaMax(&new, -beta, -alpha, depth - 1, &line);
			}
		}
		
		fifty--;
		ply--;
		repetition_index--;
		
		legals++;

		if (stopped) return(0);
		
		if (score > alpha)
		{
			hash_flag = HASH_FLAG_EXACT;

			alpha = score;
			
			pline->pv[0] = list.moves[count];
			memcpy(pline->pv + 1, line.pv, line.length*sizeof(int));
			pline->length = line.length+1;
			
			if (score >= beta)
			{
				WriteHash(pos, beta, pline->pv[0], depth, HASH_FLAG_BETA);
				
				if ((GetFlag(list.moves[count]) & CAPTURE) == 0)
				{
					killer_moves[1][ply] = killer_moves[0][ply];
					killer_moves[0][ply] = list.moves[count];
				}

				return(beta);
			}
		}
	}

	if (legals == 0)
	{
		if (in_check) return(-MATE_VALUE+ply);
		return(Contempt(pos));
	}

	WriteHash(pos, alpha, pline->pv[0], depth, hash_flag);

	return(alpha);
}

static void PrintPV(int depth)
{
	for (int count = 0; count < depth; count++)
	{
		PrintMove(pv_line.pv[count]);
		printf(" ");
	}
	printf("\n");
	return;
}

void SearchBest(POS *pos, int depth)
{
	ply = 0;
	nodes = 0;
	memset(&pv_line, 0, sizeof(LINE));
	memset(killer_moves, 0, sizeof(killer_moves));

	int start = GetTimeMs();
	int score;
	
	stopped = 0;

	for (int search_depth = 1; search_depth <= depth; search_depth++)
	{
		score = NegaMax(pos, -INFINITY, INFINITY, search_depth, &pv_line);
		
		if (stopped) break;
		
		if (!pv_line.length) continue;
		
		if (score > -MATE_VALUE && score < -MATE_SCORE)
		{
			printf("info score mate %d depth %d nodes %lld time %d pv ", -(score+MATE_VALUE)/2-1, search_depth, nodes, GetTimeMs()-start);
			PrintPV(search_depth);
			break;
		} else if (score > MATE_SCORE && score < MATE_VALUE)
		{
			printf("info score mate %d depth %d nodes %lld time %d pv ", (MATE_VALUE-score)/2+1, search_depth, nodes, GetTimeMs()-start);
			PrintPV(search_depth);
			break;
		} else
		{
			printf("info score cp %d depth %d nodes %lld time %d pv ", score, search_depth, nodes, GetTimeMs()-start);
			PrintPV(search_depth);
		}

	}

	printf("bestmove ");
	PrintMove(pv_line.pv[0]);
	printf("\n");

	return;
}
