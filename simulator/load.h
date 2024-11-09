#ifndef LOAD_H
#define LOAD_H
#define MEMORY_SIZE 16 // Instruction memory size: 2^(4 bits)

extern unsigned char IM[MEMORY_SIZE]; // Instruction memory
extern unsigned char *PC; // Program counter

typedef struct {
	unsigned char RA, RB, RO;
} Registers;

void loadToIM(const char *binFile);

#endif
