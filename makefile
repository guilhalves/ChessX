LDFLAGS=-Ofast -march=native -flto=auto -DUSE_SSE41 -msse4.1 -DUSE_SSE3 -mssse3 -DUSE_SSE2 -msse2 -DUSE_SSE -msse
CC=gcc

#ifdef WIN32
RUN=start run gen
BIN=run.exe
#else
RUN=./run gen
BIN=run
#endif

INC_DIR = ./include/
SRC_DIR = ./src/
CCFLAGS=-Wall -Wextra

SRC=$(wildcard $(SRC_DIR)*.c)
OBJ=$(SRC:.c=.o)

all: $(BIN)

$(BIN): $(OBJ)
	$(CC) -fprofile-use -o $(BIN) $(SRC_DIR)*.c $(CCFLAGS) $(LDFLAGS)                        

gen:
	$(CC) -fprofile-generate -o $(BIN) $(SRC_DIR)*.c $(CCFLAGS) $(LDFLAGS)
	$(RUN)
	$(CC) -fprofile-use -o $(BIN) $(SRC_DIR)*.c $(CCFLAGS) $(LDFLAGS)

debug:
	$(CC) -g -pg -o $(BIN) $(SRC_DIR)*.c $(CCFLAGS) $(LDFLAGS)

clean:
	rm -f $(SRC_DIR)*.o ./*.gcda $(BIN)
