#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_IMMEDIATE_VALUE 7
#define INSTRUCTION_LENGTH 8

void remove_spaces(char *str) {
	char *str2 = str;
	while (*str) {
		if (!isspace((unsigned char)*str)) {
			*str2++ = *str; // Copy non-space characters
		}
		*str++;
	}
	*str2 = '\0'; // Terminate the space-free string
}

const char* generate_machine_code(const char *instruction, char *machine_code) {
	int imm_value = 0;

	if (strcmp(instruction, "RA=RA+RB") == 0) {
		strncpy(machine_code, "00000000", INSTRUCTION_LENGTH + 1);
	} else if (strcmp(instruction, "RB=RA+RB") == 0) {
		strncpy(machine_code, "00010000", INSTRUCTION_LENGTH + 1);
	} else if (strcmp(instruction, "RA=RA-RB") == 0) {
		strncpy(machine_code, "00000100", INSTRUCTION_LENGTH + 1);
	} else if (strcmp(instruction, "RB=RA-RB") == 0) {
		strncpy(machine_code, "00010100", INSTRUCTION_LENGTH + 1);
	} else if (strcmp(instruction, "RO=RA") == 0) {
		strncpy(machine_code, "00100000", INSTRUCTION_LENGTH + 1);
	} else if (sscanf(instruction, "RA=%d", &imm_value) == 1 && imm_value <= MAX_IMMEDIATE_VALUE) {
		strncpy(machine_code, "00001III", INSTRUCTION_LENGTH + 1);
	} else if (sscanf(instruction, "RB=%d", &imm_value) == 1 && imm_value <= MAX_IMMEDIATE_VALUE) {
		strncpy(machine_code, "00011III", INSTRUCTION_LENGTH + 1);
	} else if (sscanf(instruction, "JC=%d", &imm_value) == 1 && imm_value <= MAX_IMMEDIATE_VALUE) {
		strncpy(machine_code, "01111III", INSTRUCTION_LENGTH + 1);
	} else if (sscanf(instruction, "J=%d", &imm_value) == 1 && imm_value <= MAX_IMMEDIATE_VALUE) {
		strncpy(machine_code, "10111III", INSTRUCTION_LENGTH + 1);
	} else {
		return NULL; // Unrecognized input
	}
	
	if (strchr(machine_code, 'I')) {
		for (int i = 2; i >= 0; --i) {
			machine_code[strlen(machine_code) - 1 - i] = (imm_value & (1 << i)) ? '1' : '0';
		}
	}

	return machine_code;
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
    char machine_code[INSTRUCTION_LENGTH + 1];

    while (fgets(line, sizeof(line), input_file)) {
        remove_spaces(line);
        const char *generated_code = generate_machine_code(line, machine_code);

	if (generated_code) {
            fprintf(output_file, "%s\n", generated_code);
            printf("Line %d: %s -> Machine Code: %s\n", line_number, line, generated_code);
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
    snprintf(output_filename, sizeof(output_filename), "%.*s.bin",(int)(strchr(argv[1], '.') - argv[1]), argv[1]);

    assemble(argv[1], output_filename);

    return 0;
}
