#ifndef UTILS_H
#define UTILS_H
#include <stdbool.h>
#include "control.h"
#include "load.h"

unsigned char mux(unsigned char A, unsigned char B, bool Sreg);
void execute(ControlSignals *control, Registers *regs, bool isStepByStep);
void runStepByStep(bool isStepByStep);
void runContinuous();

#endif
