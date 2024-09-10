#include "../include/uci.h"

int quit = 0;
int moves_to_go = 30;
int move_time = -1;
int time = -1;
int inc = 0;
int start_time = 0;
int stop_time = 0;
int time_set = 0;
int stopped = 0;
	
int InputWaiting()
{
	#ifndef WIN32
		fd_set readfds;
		struct timeval tv;
		FD_ZERO (&readfds);
		FD_SET (fileno(stdin), &readfds);
		tv.tv_sec=0; tv.tv_usec=0;
		select(16, &readfds, 0, 0, &tv);
		return (FD_ISSET(fileno(stdin), &readfds));
	#else
		static int init = 0, pipe;
		static HANDLE inh;
		DWORD dw;
		if (!init)
		{
			init = 1;
			inh = GetStdHandle(STD_INPUT_HANDLE);
			pipe = !GetConsoleMode(inh, &dw);
			if (!pipe)
			{
				SetConsoleMode(inh, dw & ~(ENABLE_MOUSE_INPUT|ENABLE_WINDOW_INPUT));
				FlushConsoleInputBuffer(inh);
			}
		}

		if (pipe)
		{
			if (!PeekNamedPipe(inh, NULL, 0, NULL, &dw, NULL)) return 1;
			return dw;
		} else
		{
			GetNumberOfConsoleInputEvents(inh, &dw);
			return ((dw <= 1) ? 0 : dw);
		}
	#endif
}

void ReadInput()
{
	int bytes;
	char input[256] = "", *endc;
	
	if (InputWaiting())
	{
		stopped = 1;

		do
		{
			bytes = read(fileno(stdin), input, 256);
		} while (bytes < 0);

		endc = strchr(input,'\n');

		if (endc) *endc = 0;

		if (strlen(input) > 0)
		{
			if (!strncmp(input, "quit", 4))
			{
				quit = 1;
			} else if (!strncmp(input, "stop", 4))
			{
				quit = 1;
			}
		}
	}
	return;
}

void Communicate()
{
	if (time_set == 1 && GetTimeMs() > stop_time) stopped = 1;
	ReadInput();
}

void ResetTimeControl()
{
	quit = 0;
	moves_to_go = 30;
	move_time = -1;
	time = -1;
	inc = 0;
	start_time = 0;
	stop_time = 0;
	time_set = 0;
	stopped = 0;
	return;	
}

int ParseMove(POS *pos, char *move_str)
{
	int from = (move_str[0]-'a') + (8-(move_str[1]-'0'))*8;
	int to = (move_str[2]-'a') + (8-(move_str[3]-'0'))*8;
	
	LIST list;

	GenMoves(pos, &list);
	
	for (int count = 0; count < list.end; count++)
	{
		int move = list.moves[count];
		
		if (from == GetFrom(move) && to == GetTo(move))
		{
			int flag = GetFlag(move);
			
			switch(flag)
			{
				case PR_KNIGHT:
				case PC_KNIGHT:
					if (move_str[4] == 'n') return(move);
					break;
				case PR_BISHOP:
				case PC_BISHOP:
					if (move_str[4] == 'b') return(move);
					break;
				case PR_ROOK:
				case PC_ROOK:
					if (move_str[4] == 'r') return(move);
					break;
				case PR_QUEEN:
				case PC_QUEEN:
					if (move_str[4] == 'q') return(move);
					break;
			}

			return(move);
		}
	}

	return(0);
}

void ParsePosition(POS *pos, char *command)
{
	command += 9;
	
	char *current_char;
	
	if (strncmp(command, "startpos", 8) == 0) ParseFen(pos, start_position);
	else 
	{
		current_char = strstr(command, "fen");
		if (current_char == NULL) ParseFen(pos, start_position);
		else
		{
			current_char += 4;
			ParseFen(pos, current_char);
		}
	}
	
	current_char = strstr(command, "moves");
	if (current_char != NULL)
	{
		current_char += 6;

		while (current_char)
		{
			int move = ParseMove(pos, current_char);

			if (move == 0) break;

			while (*current_char && *current_char != ' ') current_char++;

			current_char++;
			MakeMove(pos, move);
		}
	}

	return;
}

void ParseGo(POS *pos, char *command)
{
	ResetTimeControl();
	
	int depth = -1;
	
	char *argument;
	
	if ((argument = strstr(command, "binc")) && !pos->side) inc = atoi(argument + 5);
	if ((argument = strstr(command, "winc")) && pos->side) inc = atoi(argument + 5);
	
	if ((argument = strstr(command, "btime")) && !pos->side) time = atoi(argument + 6);
	if ((argument = strstr(command, "wtime")) && pos->side) time = atoi(argument + 6);

	if ((argument = strstr(command, "movestogo"))) moves_to_go = atoi(argument + 10);

	if ((argument = strstr(command, "movetime"))) move_time = atoi(argument+9);

	if ((argument = strstr(command, "depth"))) depth = atoi(argument + 6);
	
	if (move_time != -1) { time = move_time; moves_to_go = 1; };

	start_time = GetTimeMs();

	if (time != -1)
	{
		time_set = 1;
		time /= moves_to_go;
		time -= 50;

		if (time < 0)
		{
			time = 0;
			inc -= 50;
			if (inc < 0) inc = 1;
		} else
		{
			time -= 50;
		}
		stop_time = start_time + time + inc/2;
	}

	if (depth == -1) depth = MAX_PLY;

	SearchBest(pos, depth);
}

void UCILoop(POS *pos)
{
	setbuf(stdin, NULL);
	setbuf(stdout, NULL);

	char input[2000];

	int mb = 128;

	printf("id name ChessX\n");
	printf("id author Guilherme H.\n");
	
	while (1)
	{
		memset(input, 0, sizeof(input));

		fflush(stdout);

		if (!fgets(input, 2000, stdin)) continue;

		if (input[0] == '\n') continue;

		if (!strncmp(input, "isready", 7)) printf("readyok\n");

		else if (!strncmp(input, "position", 8)) { ParsePosition(pos, input); }
		
		else if (!strncmp(input, "ucinewgame", 10)) { ParsePosition(pos, "position startpos"); ClearHashTable(); }
		
		else if (!strncmp(input, "go", 2)) ParseGo(pos, input);
		
		else if (!strncmp(input, "uci", 3))
		{
			printf("id name ChessX\n");
			printf("id author Guilherme H.\n");
			printf("\noption name Hash type spin default 128 min 1 max 1024\n");
			printf("uciok\n");
		}
		
		else if (!strncmp(input, "perft", 5)) PerftTest(pos, *(input+6)-'0');

		else if (!strncmp(input, "d", 1)) PrintBoard(*pos);
		
		else if (!strncmp(input, "setoption name Hash value ", 26))
		{
			sscanf(input, "%*s %*s %*s %*s %d", &mb);

			//if (mb < 1) mb = 1;
			if (mb > 1024) mb = 1024;

			InitHashTable(mb);
		}

		else if (!strncmp(input, "stop", 4))
		{
			printf("bestmove ");
			PrintMove(pv_line.pv[0]);
			printf("\n");
		}

		else if (!strncmp(input, "quit", 4)) break;
	}

	return;
}
