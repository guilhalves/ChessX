#ifndef EVAL_H
#define EVAL_H

#include "bitboard.h"
#include "board.h"
#include "gen.h"

void InitEvaluationMasks();

int Evaluate(POS *pos);

#endif
