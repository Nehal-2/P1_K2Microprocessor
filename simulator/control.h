#ifndef CONTROL_H
#define CONTROL_H
#include <stdbool.h>

typedef struct {
	bool J, C, D1, D0, Sreg, S;
	unsigned char imm;
} ControlSignals;

typedef struct {
	bool EnA, EnB, EnO;
} EnableSignals;

void decode(unsigned char instr, ControlSignals *control);
EnableSignals demux(bool D1, bool D0);

#endif
