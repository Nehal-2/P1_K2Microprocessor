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

// Function declarations:
void loadToIM(const char *binFile);
unsigned char fetch(ControlSignals *control, Registers *regs);
void decode(unsigned char instr, ControlSignals *control);
EnableSignals demux(bool D1, bool D0);
ALUOut ALU(unsigned char RA, unsigned char RB, bool isSub);
unsigned char mux(unsigned char A, unsigned char B, bool Sreg);
void execute(ControlSignals *control, Registers *regs);

int main() {
	FILE *testBinFile;
	
	// Taking input from a test .bin file:
    	testBinFile = fopen("testBinFile.bin", "wb");

    	if (testBinFile == NULL) {
        	perror("Error! Could not open file.\n");
        	return 1;
    	}

    	unsigned char bytes[] = {0b00001000, 0b00011001, 0b00100000, 0b00010000, 
	0b01110000, 0b00000000, 0b00010100, 0b00000100, 0b10110010};
    	fwrite(bytes, sizeof(unsigned char), 9, testBinFile);
    	fclose(testBinFile);

    	loadToIM("testBinFile.bin");
    	PC = IM;

    	ControlSignals control = {0};
    	Registers regs = {0};

    	for (int i = 0; i < MEMORY_SIZE; i++) {
		unsigned char currentInstr = fetch(&control, &regs);
        	decode(currentInstr, &control);
        	execute(&control, &regs);

        	if (control.J) {
            		PC = IM + control.imm;
			//printf("Unconditional jump to address: 0x%02X\n", control.imm);
        	} else if (control.C) {
            		//printf("Carry flag is 1, performing conditional jump to address: 0x%02X\n", control.imm);
            		PC = IM + control.imm;
        	} else {
            		//printf("No jump, incrementing PC.\n");
            		PC++;
        	}
		/*
        	// Print control.C to check if it's the cause of the jump
        	printf("Control.C (Carry): %d\n", control.C);

        	printf("PC: 0x%02X\n", (unsigned int)(PC - IM));
        	printf("Registers:\nRA: 0x%02X, RB: 0x%02X, RO: 0x%02X\n", regs.RA, regs.RB, regs.RO);
        	printf("--------------------------------------------------\n");
    		*/
    	}
   	return 0;
}

void loadToIM(const char *binFile) {
	FILE *file = fopen(binFile, "rb");
	if (file == NULL) {
        	perror("Error! Could not open file.");
        	return;
    	}	
    
    	int i = 0;
    	unsigned char chByte;
    	while (fread(&chByte, sizeof(unsigned char), 1, file) == 1 && i < MEMORY_SIZE) {
        	IM[i++] = chByte;
    	}	
    	fclose(file);
}

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

ALUOut ALU(unsigned char RA, unsigned char RB, bool isSub) {
	ALUOut output;
	unsigned short result = isSub ? (RA - RB) : (RA + RB);
    	output.sum = result & 0xFF;
    	output.carry = (result >> 8) & 0x1;
    
    	if (output.carry) {
        	printf("ALU: Carry flag set to 1 (overflow detected)\n");
    	} else {
        	printf("ALU: Carry flag set to 0\n");
    	}
    
    	return output;
}

unsigned char mux(unsigned char A, unsigned char B, bool Sreg) {
	return Sreg ? B : A;
}

void execute(ControlSignals *control, Registers *regs) {
	ALUOut outALU = ALU(regs->RA, regs->RB, control->S);
    	unsigned char alu_result = mux(outALU.sum, control->imm, control->Sreg);
    	EnableSignals enSignals = demux(control->D1, control->D0);
    
    	if (enSignals.EnA) {
        	regs->RA = alu_result;
        	printf("0x%X is loaded into RA\n", regs->RA);
    	}
    	if (enSignals.EnB) {
        	regs->RB = alu_result;
        	printf("0x%X is loaded into RB\n", regs->RB);
    	}
    	if (enSignals.EnO) {
        	regs->RO = regs->RA;
        	printf("0x%X is loaded into RO\n", regs->RO);
    	}

    	control->C = outALU.carry;
    	printf("ALU Result:\n Sum = 0x%X, Carry = %d\n", outALU.sum, outALU.carry);
}
