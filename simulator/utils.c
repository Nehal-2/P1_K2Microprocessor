#include "utils.h"
#include "alu.h"
#include "control.h"
#include "fetch.h"
#include "memory.h"
#include <stdio.h>

unsigned char mux(unsigned char A, unsigned char B, bool Sreg) {
	return Sreg ? B : A;
}

void execute(ControlSignals *control, Registers *regs, bool isStepByStep) {
	// Wrap around register maximum
	regs->RA = regs->RA % (MAX_REG_VALUE + 1);
	regs->RB = regs->RB % (MAX_REG_VALUE + 1);
	regs->RO = regs->RO % (MAX_REG_VALUE + 1);

	ALUOut outALU = ALU(regs->RA, regs->RB, control->S);
    	unsigned char alu_result = mux(outALU.sum, control->imm, control->Sreg);
    	EnableSignals enSignals = demux(control->D1, control->D0);
    
    	if (enSignals.EnA) {
        	regs->RA = alu_result;
		if (isStepByStep) {
			printf("0x%X is loaded into RA\n", regs->RA);
		}
    	}
    	if (enSignals.EnB) {
        	regs->RB = alu_result;
        	if (isStepByStep) {
			printf("0x%X is loaded into RB\n", regs->RB);
		}
    	}
    	if (enSignals.EnO) {
        	regs->RO = regs->RA;
        	if (regs->RO == regs->RA && isStepByStep) {
			printf("0x%X is loaded into RO\n", regs->RO);
		}
    	}

	if (!isStepByStep) {
		if (regs->RO == regs->RA) {
			printf("RO= %d\n", regs->RO);
		}
	}

	control->C = outALU.carry;
}

void runStepByStep(bool isStepByStep) {
	printf("Starting Simulator in step-by-step mode...\n");

	ControlSignals control = {0};
	Registers regs = {0};
	int instructionCount = 0;

	while (PC < IM + MEMORY_SIZE) {
		unsigned char currentInstr = fetch(&control, &regs);
		decode(currentInstr, &control);
		execute(&control, &regs, true);
		
		if (control.J) {
            		PC = IM + control.imm;
			printf("Unconditional jump to address: 0x%02X\n", control.imm);
        	} else if (control.C) {
            		printf("Carry flag is 1, performing conditional jump to address: 0x%02X\n", control.imm);
            		PC = IM + control.imm;
        	} else {
            		printf("No jump, incrementing PC.\n");
            		PC++;
        	}

		if (regs.RO == regs.RA) {
			printf("Instruction: %d RA= %d, RB= %d,	RO= %d\n", 
			instructionCount, regs.RA, regs.RB, regs.RO);
		} else {
			printf("Instruction: %d RA= %d, RB= %d\n", instructionCount, regs.RA, regs.RB);
		}
		instructionCount++;
		printf("[Press Enter to continue]\n");
		getchar();
	}
}

void runContinuous() {
	printf("Starting Simulator in continuous mode...\n");

	ControlSignals control = {0};
	Registers regs = {0};
	
	while(PC < IM + MEMORY_SIZE) {
		for (int i = 0; i < MEMORY_SIZE; i++) {
			unsigned char currentInstr = fetch(&control, &regs);
			decode(currentInstr, &control);
			execute(&control, &regs, false);

			if (control.J) {
            			PC = IM + control.imm;
        		} else if (control.C) {
				PC = IM + control.imm;
        		} else {
            			PC++;
        		}

			printf("RO= %d\n", regs.RO);
		}
	}
}
