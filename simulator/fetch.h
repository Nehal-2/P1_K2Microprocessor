#ifndef FETCH_H
#define FETCH_H
#include "control.h"
#include "load.h"

extern unsigned char *PC;

unsigned char fetch(ControlSignals *control, Registers *regs);

#endif
