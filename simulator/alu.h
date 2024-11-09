#ifndef ALU_H
#define ALU_H
#include <stdbool.h>
#define MAX_REG_VALUE 15 // Maximum value for a 4-bit register

typedef struct {
	unsigned char sum;
	bool carry;
} ALUOut;

ALUOut ALU(unsigned char RA, unsigned char RB, bool isSub);

#endif
