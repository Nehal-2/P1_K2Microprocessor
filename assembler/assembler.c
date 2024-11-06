#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *instruction;
    char *machine_code;
} Instruction;

Instruction instructions[] = {
    {"RA=0", "00001000"},
    {"RB=1", "00011001"},
    {"RO=RA", "00100000"},
    {"RB=RA+RB", "00101000"},
    {"JC=0", "01110000"},
    {"RA=RA+RB", "00000000"},
    {"RB=RA-RB", "00010100"},
    {"RA=RA-RB", "00000100"},
    {"J=2", "10110010"}
};

#define NUM_INSTRUCTIONS (sizeof(instructions) / sizeof(Instruction))

const char* get_machine_code(const char *instruction) {
    for (int i = 0; i < NUM_INSTRUCTIONS; i++) {
        if (strcmp(instruction, instructions[i].instruction) == 0) {
            return instructions[i].machine_code;
        }
    }
    return NULL;
}

void assemble(const char *input_filename, const char *output_filename) {
    FILE *input_file = fopen(input_filename, "r");
    FILE *output_file = fopen(output_filename, "w");

    if (!input_file) {
        fprintf(stderr, "Error: Could not open input file %s\n", input_filename);
        exit(1);
    }
    if (!output_file) {
        fprintf(stderr, "Error: Could not open output file %s\n", output_filename);
        fclose(input_file);
        exit(1);
    }

    char line[256];
    int line_number = 0;
    int error_found = 0;
    while (fgets(line, sizeof(line), input_file)) {
        line[strcspn(line, "\n")] = 0;
        const char *machine_code = get_machine_code(line);

        if (machine_code) {
            fprintf(output_file, "%s\n", machine_code);
            printf("Line %d: %s -> Machine Code: %s\n", line_number, line, machine_code);
        } else {
            fprintf(stderr, "Error on line %d: Unknown instruction or register %s\n", line_number, line);
            error_found = 1;
        }

        line_number++;
    }

    fclose(input_file);
    fclose(output_file);

    if (error_found) {
        printf("Assembly completed with errors.\n");
    } else {
        printf("Successfully generated output file: %s\n", output_filename);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <input_file.asm>\n", argv[0]);
        return 1;
    }

    char output_filename[256];
    snprintf(output_filename, sizeof(output_filename), "%s.bin", argv[1]);

    assemble(argv[1], output_filename);

    return 0;
}
