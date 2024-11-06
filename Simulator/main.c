#include <stdio.h>
#include <stdbool.h>

#define MEMORY_SIZE 16 // Instruction memory size: 2^(4 bits)

unsigned char IM[MEMORY_SIZE]; // Instruction memory
unsigned char *PC; // Program counter

typedef struct {
	unsigned char sum;
	bool carry;
} ALUOut;

typedef struct {
	bool J, C, D1, D0, Sreg, S;
	unsigned char imm;
} ControlSignals;

typedef struct {
	unsigned char RA, RB, RO;
} Registers;

typedef struct {
	bool EnA, EnB, EnO;
} EnableSignals;

// Functions' declarations:
void loadToIM(const char *binFile);
unsigned char fetch(ControlSignals *control, Registers *regs);
void decodeInstr(unsigned char instr, ControlSignals *control);
EnableSignals demux(bool D1, bool D0);
ALUOut ALU(unsigned char A, unsigned char B);
int mux(int A, int B, bool Sreg);
void execute(ControlSignals *control, Registers *regs);

int main() {
	FILE *testBinFile;
	// Start of writing bin file:
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
	// End of writing bin file
	loadToIM("testBinFile.bin");
	//loadToIM("/home/it/P1/fibonacci.bin");
	PC = IM;

	ControlSignals control = {0};
	Registers regs = {0};

	for (int i = 0; i < MEMORY_SIZE; i++) {
		unsigned char currentInstr = *PC;
		decodeInstr(currentInstr, &control);
		fetch(&control, &regs);
		PC++;
		printf("Instruction %d: 0x%02X\n", i, IM[i]);
    	}	
	
	return 0;
}

//Functions' definitions:
void loadToIM(const char *binFile) {
	FILE *file = fopen(binFile, "rb");
	if (file == NULL) {
		perror("Error! Could not open file.");
		return;
	}
	
	int i = 0;
	unsigned char chByte;

	while (fread(&chByte, sizeof(unsigned char), 1, file) == 1 
	&& i < MEMORY_SIZE) {
		IM[i] = chByte;
		i++;
	}

/*
		for (int iBit = 0; iBit < 8; iBit++) {
			IM[i] <<= 1; // Shift left by 1 to make room for the new bit
			if ((chByte >> (7 - iBit)) & 1) { // Check if the current bit is 1
				IM[i] |= 0x01; // Set the LSB of IM[i] to 1
			} 
		}	*/
	fclose(file);
}

unsigned char fetch(ControlSignals *control, Registers *regs) {
	unsigned char instr = *PC;

	if (control->J) {
		PC = IM + control->imm;
	} else if (control->C && control->D1 && control->D0) {
		PC = IM + control->imm;
	} else {
		PC++;
	}

	return instr;
}
	
void decodeInstr(unsigned char instr, ControlSignals *control) {
	control->J = (instr & 0x80) ? 1 : 0;
	control->C = (instr & 0x40) ? 1 : 0;
	control->D1 = (instr & 0x20) ? 1 : 0;
	control->D0 = (instr & 0x10) ? 1 : 0;
	control->Sreg = (instr & 0x08) ? 1 : 0;

	control->imm = 0; // Clear imm at the beginning of each instruction

	// Check if the instruction requires the 6th bit as S-flag or imm[2]
	if (control->Sreg) { // If Sreg = 1, treat the 6th bit as the S flag
		control->S = (instr & 0x20) ? 1 : 0;
		control->imm |= ((instr & 0x10) >> 4);
	} else { // If Sreg != 1, treat the 6th bit as imm[2]
		control->imm |= ((instr & 0x20) >> 5); // Combine the last 3 bits with 6th bit as imm[2]
		control->imm |= ((instr & 0x10) >> 4);
		control->imm |= ((instr & 0x08) >> 3);
	}

	printf("Control Signals:\n");
    	printf("J: %d, C: %d, D1: %d, D0: %d, Sreg: %d, S: %d, Imm:%d\n",
          control->J, control->C, control->D1, control->D0, control->Sreg, 
	  control->S, control->imm);
}

EnableSignals demux(bool D1, bool D0) {
	EnableSignals enSignals;
	enSignals.EnA = D1 && D0;
	enSignals.EnB = D1 && !D0;
	enSignals.EnO = !D1 && D0;

	return enSignals;
}

ALUOut ALU(unsigned char RA, unsigned char RB) {
	ALUOut output;
	unsigned short result = RA + RB;
	output.sum = result & 0xFF; // Mask to 8 bits
	output.carry = (result >> 8) & 0x1; // Extract carry

		return output;
}

int mux(int A, int B, bool Sreg) {
	return Sreg ? B : A;
}

void execute(ControlSignals *control, Registers *regs) {
	unsigned char temp = mux(ALU(regs->RA, regs->RB).sum, control->imm, control->Sreg);
	EnableSignals enSignals = demux(control->D1, control->D0);
	
	// Update registers:
	if (enSignals.EnA) {
		regs->RA = temp;
		printf("0x%X is loaded into RA\n", regs->RA);
	}
	if (enSignals.EnB) {
		regs->RB = temp;
		printf("0x%X is loaded into RB\n", regs->RB);
	}
	if (enSignals.EnO) {
		regs->RO = regs->RA;
		printf("0x%X is loaded into RO)\n", regs->RO);
	}
	
	ALUOut outALU = ALU(regs->RA, regs->RB);
	printf("ALU Result:\n Sum = 0x%X, Carry = %d\n", outALU.sum, outALU.carry);

	control->C = outALU.carry; // Update the carry flag
}
