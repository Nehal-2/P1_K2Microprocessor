#include "fetch.h"
#include "memory.h"

unsigned char fetch(ControlSignals *control, Registers *regs) {
    	unsigned char instr = *PC;
    	control->J = (instr >> 7) & 0x01;
    	control->C = (instr >> 6) & 0x01;
    	control->D1 = (instr >> 5) & 0x01;
    	control->D0 = (instr >> 4) & 0x01;
    	control->Sreg = (instr >> 3) & 0x01;
    	control->S = (instr >> 2) & 0x01;
    	control->imm = instr & 0x07;
    	
	return instr;
}
