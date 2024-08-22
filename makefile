LDFLAGS=-Ofast -flto=auto -DUSE_SSE41 -msse4.1 -DUSE_SSE3 -mssse3 -DUSE_SSE2 -msse2 -DUSE_SSE -msse

WINCC=x86_64-w64-mingw32-gcc
CC=gcc

WINRUN=wine run gen
WINBIN=ChessX.exe
RUN=./run gen
BIN=chessx

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
	./$(BIN) gen
	$(CC) -fprofile-use -o $(BIN) $(SRC_DIR)*.c $(CCFLAGS) $(LDFLAGS)
	
	$(WINCC) -fprofile-generate -o $(WINBIN) $(SRC_DIR)*.c $(CCFLAGS) $(LDFLAGS)
	wine $(WINBIN) gen
	$(WINCC) -fprofile-use -o $(WINBIN) $(SRC_DIR)*.c $(CCFLAGS) $(LDFLAGS)
	
debug:
	$(CC) -g -pg -o $(BIN) $(SRC_DIR)*.c $(CCFLAGS) $(LDFLAGS)

clean:
	rm -f $(SRC_DIR)*.o ./*.gcda $(BIN) $(WINBIN)
