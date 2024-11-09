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
	int jump_condition;
	char* reg_address;
	char* alu_op;
	int custom_input;
	int Sreg;
} DecodeInstr;

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
DecodeInstr decodeInstr(unsigned char instr);
//void decodeInstr(unsigned char instr, ControlSignals *control);
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
		unsigned char currentInstr = fetch(&control, &regs);
		DecodeInstr decoded = decodeInstr(currentInstr);

		printf("Instr: 0b%08b\n", bytes[i]);
		printf("Jump Condition: %d\n", decoded.jump_condition);
   		 printf("Register Address: %s\n", decoded.reg_address ? decoded.reg_address : "None");
   		printf("ALU Operation: %s\n", decoded.alu_op);
    		printf("Custom Input: %d\n", decoded.custom_input);
    		printf("ALU or Custom Input: %d\n", decoded.Sreg);

		//printf("Instruction %d: 0x%02X\n", i, currentInstr);

		if (control.J) {
			PC = IM + control.imm;
			printf("Jumping to address: 0x%02X\n", control.imm);
		} else if (control.C && control.D1 && control.D0) {
			PC = IM + control.imm;
			printf("Conditional jump to address: 0x%02X\n", control.imm);
		} else {
			PC++;
		}	
		execute(&control, &regs);

		// Print current PC and register states
        	printf("PC: 0x%02X\n", (unsigned int)(PC - IM)); // Print PC as an offset from the start of IM
        	printf("Registers:\n");
        	printf("RA: 0x%02X, RB: 0x%02X, RO: 0x%02X\n", regs.RA, regs.RB, regs.RO);
        	printf("--------------------------------------------------\n");
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
		IM[i++] = chByte;
	}
	/*
		IM[i] = 0;
		for (int iBit = 0; iBit < 8; iBit++) {
			if (chByte & (1 << (7 - iBit))) {
				IM[i] |= (1 << (7 - iBit));
			}
		}
		i++;
	}
	fclose(file);

		IM[i] = chByte;
		for (int iBit = 0; iBit < 8; iBit++) {
			if (IM[i] == '1') {
				chByte = chByte | (1 << (7 - iBit));
			}
		}
		//IM[i] = chByte;
		i++;
	}*/
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
/* Previous attempt of decoding
void deocdeInstr(unsigned char instr, ControlSignals *control) {

	control->J = (instr & 0x80) ? 1 : 0;
	control->C = (instr & 0x40) ? 1 : 0;
	control->D1 = (instr & 0x20) ? 1 : 0;
	control->D0 = (instr & 0x10) ? 1 : 0;
	control->Sreg = (instr & 0x08) ? 1 : 0;
	control->S = (instr & 0x04) ? 1 : 0;

	control->imm = instr & 0x03; // Clear imm at the beginning of each instruction

	// Check if the instruction requires the 6th bit as S-flag or imm[2]
	if (control->Sreg) { // If Sreg = 1, treat the 6th bit as imm[2]
		control->imm = instr & 0x07;
	} else { // If Sreg != 1, treat the 6th bit as the S flag
		control->S = (instr & 0x04) ? 1 : 0;
		control->imm = instr & 0x03;
	}
	printf("Control Signals:\n");
    	printf("J: %d, C: %d, D1: %d, D0: %d, Sreg: %d, S: %d, Imm:0x%02X\n"
	,control->J, control->C, control->D1, control->D0, control->Sreg, 
	control->S, control->imm);
	} */  // End of prev attempt


DecodeInstr decodeInstr(unsigned char instr) {
	DecodeInstr decoded;
	int J = (instr & 0x80) ? 1 : 0;
	int C = (instr & 0x40) ? 1 : 0;
	int D1 = (instr & 0x20) ? 1 : 0;
	int D0 = (instr & 0x10) ? 1 : 0;
	int Sreg = (instr & 0x08) ? 1 : 0;
	int S = (instr & 0x04) ? 1 : 0;
	int imm = instr & 0x07;
/*	// Different method
	int J = (instr >> 7) & 0x01;
	int C = (instr >> 6) & 0x01;
	int D1 = (instr >> 5) & 0x01;
	int D0 = (instr >> 4) & 0x01;
	int Sreg = (instr >> 3) & 0x01;
	int S = (instr >> 2) & 0x01;
	int imm = instr & 0x07;
*/	
	// Default
	decoded.jump_condition = 0;
	decoded.reg_address = NULL;
	decoded.alu_op = NULL;
	decoded.custom_input = imm;
	decoded.Sreg = Sreg;

	if ( J == 0) {
		decoded.jump_condition = 1;
	} else if ( C == 1) {
		decoded.jump_condition = 1;
	}

	if (D1 == 0 && D0 == 0) {
		decoded.reg_address = "RA";
	} else if (D1 == 0 && D0 == 1) {
		decoded.reg_address = "RB";
	} else if (D1 == 1 && D0 == 0) {
		decoded.reg_address = "RO";
	} else {
		decoded.reg_address = NULL;
	}

	if (S == 0) {
		decoded.alu_op = "ADD";
	} else {
		decoded.alu_op = "SUB";
	}

	return decoded;
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
	printf("Control Signals: J: %d, C: %d, D1: %d, D0: %d, Sreg: %d, S: %d, Imm: 0x%02X\n",
       control->J, control->C, control->D1, control->D0, control->Sreg, control->S, control->imm);

	control->C = outALU.carry; // Update the carry flag
}
