#include <stdio.h>
#include <stdbool.h>

#define MEMORY_SIZE 16 // Instruction memory size: 2^(4 bits)

unsigned char IM[MEMORY_SIZE]; // Instruction memory
unsigned char *PC; // Program counter

/* // Control signals:
bool J = 0;
bool C = 0;
bool D1 = 0;
bool D0 = 0;
bool Sreg = 0;
unsigned char S = 0;
bool EnA = 0;
bool EnB = 0;
bool EnO = 0;

unsigned char imm = 0;
unsigned char RA = 0;
unsigned char RB = 0;
unsigned char RO = 0;
*/
typedef struct {
	unsigned char sum;
	bool carry;
} ALUOut;

typedef struct {
	bool J, C, D1, D0, Sreg;
	unsigned char S, imm;
} ControlSignals;

typedef struct {
	unsigned char RA, RB, RO;
} Registers;

typedef struct {
	bool EnA, EnB, EnO;
} EnableSignals;

// Functions' declarations:
void loadToIM(const char *binFile);
unsigned char fetch();
void decodeInstr(unsigned char instr);
EnableSignals demux(bool D1, bool D0);
ALUOut ALU(unsigned char A, unsigned char B);
int mux(int A, int B, bool Sreg);
void execute();

int main() {
	FILE *testBinFile;
	// Start of writing bin file:
	testBinFile = fopen("testBinFile.bin", "wb");

	if (testBinFile == NULL) {
		perror("Error! Could not open file.\n");
		return 1;
	}
	
	unsigned char byte1 = 0b00000000;
    	unsigned char byte2 = 0b00010000;

    	fwrite(&byte1, sizeof(unsigned char), 1, testBinFile);
    	fwrite(&byte2, sizeof(unsigned char), 1, testBinFile);
    	fclose(testBinFile);
	// End of writing bin file

	loadToIM("testBinFile.bin");
	PC = IM;

	for (int i = 0; i < MEMORY_SIZE; i++) {
		unsigned char currentInstr = *PC;
		decodeInstr(currentInstr);
		PC++;
		//printf("Instruction %d: 0x%02X\n", i, IM[i]);
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

	while (fread(&chByte, sizeof(unsigned char), 1, file) == 1 
	&& i < MEMORY_SIZE) { 
		for (int iBit = 0; iBit < 8; iBit++) {
			if ((chByte >> (7 - iBit)) & 1) {
				IM[i] = (IM[i] >> 1) | 0x80;
			} else {
				IM[i] = IM[i] >> 1;
			}
		}	
		i++;
	}
	fclose(file);
}

unsigned char fetch() {
	unsigned char instr = *PC;

	if (J) {
		PC = IM + imm;
	} else if (J && C) {
		PC = IM + imm;
	} else {
		PC++;
	}

	return instr;
}
	
void decodeInstr(unsigned char instr) {

	J = (instr & 0x80) ? 1 : 0;
	C = (instr & 0x40) ? 1 : 0;
	D1 = (instr & 0x20) ? 1 : 0;
	D0 = (instr & 0x10) ? 1 : 0;
	Sreg = (instr & 0x08) ? 1 : 0;

	// S is custom input (3 bits: 2, 1, and 0)
	S = instr & 0x07;
	printf("Control Signals:\n");
    	printf("J: %d, C: %d, D1: %d, D0: %d, Sreg: %d, S: 0x%X\n",
           J, C, D1, D0, Sreg, S);
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
	unsigned short result = 0;
	int op = 0; 
	switch (op) {
		case 0: //RA = RA + RB
			result = RA + RB;
			output.sum = result & 0xFF; // Mask to 8 bits
			output.carry = (result >> 8) & 0x1; // Move carry to be the 9th bit
			break;
		case 1: // RB = RA + RB
			result = RA + RB;
			output.sum = result & 0xFF; // Mask to 8 bits
			output.carry = (result >> 8) & 0x1; // Move carry to be the 9th bit
			break;
		case 2: // RA = RA - RB
			result = RA - RB;
			output.sum = result & 0xFF; // Mask to 8 bits
			output.carry = (result > RA) ? 1 : 0;
			break; 
		case 3: // RB = RA - RB
			result = RA - RB;
			output.sum = result & 0xFF; // Mask to 8 bits
			output.carry = (result > RA) ? 1 : 0;
			break;
		case 4: // RO = RA
			output.sum = RA;
			output.carry = 0;
			break;
		case 5: // RA = imm
			output.sum = imm;
			output.carry = 0;
			break;
		case 6: // RB = imm
			output.sum = imm;
			output.carry = 0;
			break;
		case 7: // JC = imm
			output.sum = 0;
			output.carry = 0;
			break;
		case 8: // J = imm
			output.sum = 0;
			output.carry = 0;
			break;
	}

		return output;
}

int mux(int A, int B, bool Sreg) {
	return Sreg ? B : A;
}

void execute() {
	unsigned char temp = mux(ALU(RA, RB).sum, imm, Sreg);
	EnableSignals enSignals = demux(D1, D0);
	
	// Update registers:
	if (enSignals.EnA) {
		RA = temp;
		printf("0x%X is loaded into RA\n", RA);
	}
	if (enSignals.EnB) {
		RB = temp;
		printf("0x%X is loaded into RB\n", RB);
	}
	if (enSignals.EnO) {
		RO = RA;
		printf("0x%X is loaded into RO)\n", RO);
	}
	
	ALUOut outALU = ALU(RA, RB);
	printf("ALU Result:\n Sum = 0x%X, Carry = %d\n", outALU.sum, outALU.carry);

	C = outALU.carry; // Update the carry flag
}
