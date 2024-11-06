CC = gcc
ASSEMBLER_SRC = assembler.c
SIMULATOR_SRC = simulator.c
ASSEMBLER_EXEC = assembler_exec
SIMULATOR_EXEC = simulator_exec
FILENAME ?= fibonacci.asm

assemble: $(ASSEMBLER_SRC)
	$(CC) $(ASSEMBLER_SRC) -o $(ASSEMBLER_EXEC)
	./$(ASSEMBLER_EXEC) $(FILENAME)

simulate: $(SIMULATOR_SRC)
	$(CC) $(SIMULATOR_SRC) -o $(SIMULATOR_EXEC)
	./$(SIMULATOR_EXEC) $(FILENAME)

all: assemble simulate

clean:
	rm -f $(ASSEMBLER_EXEC) $(SIMULATOR_EXEC) *.bin

help:
	@echo "Available commands:"
	@echo "  make assemble FILENAME=<filename>  - Compile and run the assembler with the specified assembly file"
	@echo "  make simulate FILENAME=<filename>  - Compile and run the simulator with the specified binary file"
	@echo "  make all                           - Compile both assembler and simulator"
	@echo "  make clean                         - Remove compiled files"
	@echo "  make help                          - Display this help message"
