#include "alu.h"
#include "memory.h"

ALUOut ALU(unsigned char RA, unsigned char RB, bool isSub) {
	ALUOut output;
	unsigned short result = isSub ? (RA - RB) : (RA + RB);
    	output.sum = result & 0xFF;
	output.sum = output.sum % (MAX_REG_VALUE + 1);
    	output.carry = (result >> 8) & 0x1;

    	return output;
}
