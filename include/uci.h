#ifndef UCI_H
#define UCI_H

#include "board.h"
#include "gen.h"
#include "utils.h"
#include "search.h"
#include <unistd.h>

void Communicate();

int ParseMove(POS *pos, char *move_str);

void ParsePosition(POS *pos, char *command);

void ParseGo(POS *pos, char *command);

void UCILoop(POS *pos);

extern int time_set;
extern int stop_time;
extern int stopped;
extern int quit;
extern int moves_to_go;
extern int move_time;
extern int time;
extern int inc;
extern int start_time;

#endif
