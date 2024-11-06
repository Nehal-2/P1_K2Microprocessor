#include <stdio.h>
#include <stdbool.h>

#define MEMORY_SIZE 16 // Instruction memory size: 2^(4 bits)

unsigned char IM[MEMORY_SIZE]; // Instruction memory
unsigned char *PC; // Program counter

// Control signals:
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

typedef struct {
	unsigned char sum;
	bool carry;
} ALUOut;

typedef struct {
	bool J, C, D1, D0, Sreg;
	unsigned char S;
} ControlSignals;

typedef struct {
	bool EnA, EnB, EnO;
} EnableSignals;

// Functions' declarations:
void loadToIM(const char *binFile);
unsigned char fetch();

EnableSignals decodeInstr(bool D1, bool D0);
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
		decode(currentInstr);
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
	} else if (JC && C) {
		PC = IM + imm;
	} else {
		PC++;
	}

	return instr;
}

//EnableSignals decode(bool D1, bool D0) {
	
void decode(unsigned char instr) {

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
