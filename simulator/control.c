#include "control.h"
#include "memory.h"

void decode(unsigned char instr, ControlSignals *control) {
	control->J = (instr >> 7) & 0x01;
    	control->C = (instr >> 6) & 0x01;
    	control->D1 = (instr >> 5) & 0x01;
    	control->D0 = (instr >> 4) & 0x01;
    	control->Sreg = (instr >> 3) & 0x01;
    	control->S = (instr >> 2) & 0x01;
    	control->imm = instr & 0x07;
}

EnableSignals demux(bool D1, bool D0) {
	EnableSignals enSignals;
    	enSignals.EnA = !D1 && !D0;
    	enSignals.EnB = !D1 && D0;
    	enSignals.EnO = D1 && !D0;
    
    	return enSignals;
}
