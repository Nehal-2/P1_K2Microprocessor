#include <stdio.h>
#include "control.h"
#include "load.h"
#include "fetch.h"
#include "utils.h"
#include "memory.h"

#define MEMORY_SIZE 16 // Instruction memory size: 2^(4 bits)
#define MAX_REG_VALUE 15 // Maximum value for a 4-bit register

int main() {
	FILE *testBinFile;
	char mode;

	// Taking input from a test .bin file:
    	testBinFile = fopen("testBinFile.bin", "wb");

    	if (testBinFile == NULL) {
        	perror("Error! Could not open file.\n");
        	return 1;
    	}

	unsigned char bytes[] = {0b00001000, 0b00011001, 0b00100000, 
	0b00010000, 0b01110000, 0b00000000, 0b00010100, 0b00000100, 
	0b10110010};
    	fwrite(bytes, sizeof(unsigned char), 9, testBinFile);
    	fclose(testBinFile);

    	loadToIM("testBinFile.bin");
    	PC = IM;

    	ControlSignals control = {0};
    	Registers regs = {0};

    	printf("Select one of the following mode\nR - Run in continuous mode\nS - Run step-by-step\nSelect mode: ");
	scanf(" %c", &mode);

	bool isStepByStep = (mode == 'S' || mode == 's');
	if (isStepByStep) {
		runStepByStep(isStepByStep);
	} else {
		runContinuous();
	}
	return 0;
}
