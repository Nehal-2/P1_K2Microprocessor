CC = gcc
CFLAGS = -Wall -Wextra -g
SRC_DIR_SIM = simulator
SRC_DIR_ASM = assembler
OBJ_DIR_SIM = obj/simulator
OBJ_SIR_ASM = obj/assembler
BIN_DIR = bin

ASSEMBLER_SRC = $(SRC_DIR_ASM)/assembler.c
SIMULATOR_SRC = $(SRC_DIR_SIM)/main.c $(SRC_DIR_SIM)/utils.c $(SRC_DIR_SIM)/fetch.c $(SRC_DIR_SIM)/alu.c $(SRC_DIR_SIM)/control.c $(SRC_DIR_SIM)/load.c $(SRC_DIR_SIM)/memory.c
ASSEMBLER_OBJ = $(ASSEMBLER_SRC: .c=.o)
SIMULATOR_OBJ = $(SIMULATOR_SRC: .c=.o)
ASSEMBLER_EXEC = $(SRC_DIR_ASM)/assembler
SIMULATOR_EXEC = $(SRC_DIR_SIM)/simulator
FILENAME ?= $(SRC_DIR_ASM)/fibonacci.asm

# Default target: all
all: assemble simulate

assemble: $(ASSEMBLER_EXEC)
	./$(ASSEMBLER_EXEC) $(FILENAME)

$(ASSEMBLER_EXEC): $(ASSEMBLER_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(ASSEMBLER_OBJ) -o $(ASSEMBLER_EXEC)

$(ASSEMBLER_OBJ): $(SRC_DIR_ASM)/%.o : $(SRC_DIR_ASM)/%.c
	@mkdir -p $(OBJ_DIR_ASM)
	$(CC) $(CFLAGS) -c $< -o $(OBJ_DIR_ASM)/$@

simulate: $(SIMULATOR_EXEC)
	./$(SIMULATOR_EXEC) $(FILENAME)

$(SIMULATOR_EXEC): $(SIMULATOR_OBJ)
	@mkdir -p $(BIN_DIR)
	$(CC) $(SIMULATOR_SRC) -o $(SIMULATOR_EXEC)

$(SIMULATOR_OBJ): $(SRC_DIR_SIM)/%.o : $(SRC_DIR_SIM)/%.c
	@mkdir -p $(OBJ_DIR_SIM)
	$(CC) $(CFLAGS) -c $< -o $(OBJ_DIR_SIM)/$@

clean:
	rm -rf $(OBJ_DIR_SIM) $(OBJ_DIR_ASM) $(BIN_DIR)/$(ASSEMBLER_EXEC) $(BIN_DIR)/$(SIMULATOR_EXEC)

help:
	@echo "Available commands:"
	@echo "  make assemble FILENAME=<filename>  - Compile and run the assembler with the specified assembly file"
	@echo "  make simulate FILENAME=<filename>  - Compile and run the simulator with the specified binary file"
	@echo "  make all                           - Compile both assembler and simulator"
	@echo "  make clean                         - Remove compiled files"
	@echo "  make help                          - Display this help message"
