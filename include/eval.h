#ifndef EVAL_H
#define EVAL_H

#include "bitboard.h"
#include "board.h"
#include "gen.h"

#define OPENING 0
#define ENDGAME 1
#define MIDDLE_GAME 2

void InitEvaluationMasks();

int GetGamePhaseScore(POS *pos, int *game_phase);

int Evaluate(POS *pos);

extern int material_score[2][6];

#endif
