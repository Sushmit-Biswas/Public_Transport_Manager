# Compiler to use
CC = gcc 

# Compiler flags: include headers from "include" and enable all warnings
CFLAGS = -Iinclude -Wall

# Source files
SRC = src/main.c src/routes.c src/schedule.c src/auth.c src/booking.c

# Convert source files (.c) to object files (.o)
OBJ = $(SRC:.c=.o)

# Output binary file (final executable)
BIN = bin/transport

# Default target: build the binary
# Print success message
# Color codes: https://en.wikipedia.org/wiki/ANSI_escape_code#Colors
all: $(BIN)
	@echo "\033[1;38;5;51m\033[48;5;236mSuccessfully created executable.\033[0m\n"
	@echo "\033[1;38;5;213m\033[48;5;236mRun './bin/transport' to start the program\033[0m\n"
	@echo "\033[1;38;5;227m\033[48;5;236mRun 'make clean' to remove object files and executable\033[0m\n"
	@echo "\033[1;38;5;84m\033[48;5;236mRun 'make' to build the program again\033[0m\n"
	@echo "\033[1;38;5;219m\033[48;5;236mThank you 🩷  for using our Public Transport Route and Schedule Management System!\033[0m\n"

# Link object files to create the final binary
$(BIN): $(OBJ)
# Ensure the bin directory exists
	@mkdir -p bin  
# Link object files into the binary
	@echo "\033[1;38;5;117m\033[48;5;236mLinking object files to create executable...\033[0m\n"
	@$(CC) $(OBJ) -o $(BIN)

# Compile source files into object files
%.o: %.c
	@echo "\033[1;38;5;156m\033[48;5;236mCompiling $<...\033[0m\n"
	@$(CC) $(CFLAGS) -c $< -o $@

# Clean up build files
clean:
# Remove object files and the binary
	@echo "\033[1;38;5;203m\033[48;5;236mCleaning up build files...\033[0m\n"
	@rm -f $(OBJ) $(BIN)
	@echo "\033[1;38;5;191m\033[48;5;236mClean complete - all object files and executable removed\033[0m\n"
