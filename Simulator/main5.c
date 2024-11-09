#include <stdio.h>
#include <stdbool.h>

#define MEMORY_SIZE 16 // Instruction memory size: 2^(4 bits)
#define MAX_REG_VALUE 13 // Maximum value for the 4-bit register (0 to 13)

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
void execute(ControlSignals *control, Registers *regs, bool *aluCarry);
void runStepByStep();
void runContinuous();
void printRO(Registers *regs);
void wrapAround(Registers *regs);

int main() {
    char mode;
    FILE *testBinFile;

    testBinFile = fopen("testBinFile.bin", "wb");

    if (testBinFile == NULL) {
        perror("Error! Could not open file.\n");
        return 1;
    }

    unsigned char bytes[] = {0b00001000, 0b00011001, 0b00100000, 
                             0b00010000, 0b01110000, 0b00000000, 
                             0b00010100, 0b00000100, 0b10110010};
    fwrite(bytes, sizeof(unsigned char), 9, testBinFile);
    fclose(testBinFile);

    loadToIM("testBinFile.bin");
    PC = IM;

    printf("Select one of the following mode\n");
    printf("R - Run in continuous mode\n");
    printf("S - Run step-by-step\n");
    printf("Select mode: ");
    scanf(" %c", &mode); // Read the mode (R or S)

    if (mode == 'S' || mode == 's') {
        runStepByStep();
    } else if (mode == 'R' || mode == 'r') {
        runContinuous();
    } else {
        printf("Invalid mode selected!\n");
    }

    return 0;
}

// Load binary instructions into IM
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

// Fetch instruction and update control signals
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

// Decode instruction and set control signals
void decode(unsigned char instr, ControlSignals *control) {
    control->J = (instr >> 7) & 0x01;
    control->C = (instr >> 6) & 0x01;
    control->D1 = (instr >> 5) & 0x01;
    control->D0 = (instr >> 4) & 0x01;
    control->Sreg = (instr >> 3) & 0x01;
    control->S = (instr >> 2) & 0x01;
    control->imm = instr & 0x07;
}

// Demux function to handle register enable signals
EnableSignals demux(bool D1, bool D0) {
    EnableSignals enSignals;
    enSignals.EnA = !D1 && !D0;
    enSignals.EnB = !D1 && D0;
    enSignals.EnO = D1 && !D0;
    return enSignals;
}

// ALU operation
ALUOut ALU(unsigned char RA, unsigned char RB, bool isSub) {
    ALUOut output;
    unsigned short result = isSub ? (RA - RB) : (RA + RB);
    output.sum = result & 0xFF;
    output.sum = output.sum % (MAX_REG_VALUE + 1);
    output.carry = (result >> 8) & 0x1;
    
    return output;
}

// Multiplexer for choosing data
unsigned char mux(unsigned char A, unsigned char B, bool Sreg) {
    return Sreg ? B : A;
}

// Execute function to simulate one step of the operation
void execute(ControlSignals *control, Registers *regs, bool *aluCarry) {
    ALUOut outALU = ALU(regs->RA, regs->RB, control->S);
    unsigned char alu_result = mux(outALU.sum, control->imm, control->Sreg);
    EnableSignals enSignals = demux(control->D1, control->D0);
    
    if (enSignals.EnA) {
        regs->RA = alu_result;
        printf("RA = 0x%02X\n", regs->RA);
    }
    if (enSignals.EnB) {
        regs->RB = alu_result;
        printf("RB = 0x%02X\n", regs->RB);
    }
    if (enSignals.EnO) {
        regs->RO = regs->RA;
        printf("RO = 0x%02X\n", regs->RO);
    }

    // Update the carry flag for conditional jumps
    *aluCarry = outALU.carry;
    printf("ALU Result:\n Sum = 0x%X, Carry = %d\n", outALU.sum, outALU.carry);
}

// Print RO when updated
void printRO(Registers *regs) {
    if (regs->RO == regs->RA) {
        printf("RO = RA -> RO = 0x%02X\n", regs->RO);
    }
}

// Wraparound logic to enforce register size limit
void wrapAround(Registers *regs) {
    regs->RA %= (MAX_REG_VALUE + 1);
    regs->RB %= (MAX_REG_VALUE + 1);
    regs->RO %= (MAX_REG_VALUE + 1);
}

// Run step-by-step
void runStepByStep() {
    printf("Starting Simulator in step-by-step mode...\n");

    ControlSignals control = {0};
    Registers regs = {0};
    bool aluCarry = 0;
    int instructionCount = 0;

    while (PC < IM + MEMORY_SIZE) {
        unsigned char currentInstr = fetch(&control, &regs);
        decode(currentInstr, &control);

        // Execute instruction
        execute(&control, &regs, &aluCarry);
        
        // Wrap around values to ensure they don't exceed 13
        wrapAround(&regs);

        // Print RO only when RO = RA
        printRO(&regs);

        // Print the instruction and wait for user input to continue
        printf("Instruction %d: RA=0x%02X RB=0x%02X RO=0x%02X\n", instructionCount, regs.RA, regs.RB, regs.RO);
        instructionCount++;
        
        if (control.J) {
            printf("JUMP: Jump to Instruction %d\n", control.imm);
            PC = IM + control.imm;
        } else {
            PC++;
        }

        printf("Press Enter to continue...\n");
        getchar(); // Wait for Enter key
    }
}

// Run in continuous mode
void runContinuous() {
    printf("Starting Simulator in continuous mode...\n");

    ControlSignals control = {0};
    Registers regs = {0};
    bool aluCarry = 0;

    while (PC < IM + MEMORY_SIZE) {
        unsigned char currentInstr = fetch(&control, &regs);
        decode(currentInstr, &control);

        // Execute instruction
        execute(&control, &regs, &aluCarry);

        // Wrap around values to ensure they don't exceed 13
        wrapAround(&regs);

        // Print RO when updated
        printRO(&regs);

        // Jump if needed
        if (control.J) {
            PC = IM + control.imm;
        } else {
            PC++;
        }
    }
}
