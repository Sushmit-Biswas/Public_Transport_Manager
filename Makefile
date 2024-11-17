CC = gcc
CFLAGS = -Iinclude -Wall
SRC = src/main.c src/routes.c src/schedule.c src/auth.c
OBJ = $(SRC:.c=.o)
BIN = bin/transport

all: $(BIN)

$(BIN): $(OBJ)
	@mkdir -p bin
	$(CC) $(OBJ) -o $(BIN)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(BIN)