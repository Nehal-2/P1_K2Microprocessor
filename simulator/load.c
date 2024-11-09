#include "load.h"
#include "memory.h"
#include <stdio.h>
#include <stddef.h>

void loadToIM(const char *filename) { 
	FILE *file = fopen(filename, "rb");
	if (file == NULL) {
        	fprintf(stderr, "Error: Could not open file %s\n", filename);
        	return;
    	}

	size_t read_size = fread(IM, sizeof(unsigned char), MEMORY_SIZE, file);
	if (read_size < MEMORY_SIZE) {
		for (size_t i = read_size; i < MEMORY_SIZE; i++) {
			IM[i] = 0;
		}
		printf("Warning: the file is smaller than MEMORY_SIZE. %ld bytes are loaded to the instruction memory and the rest are initialized with zeros.\n", read_size);
	}
	printf("Loaded file '%s' into instruction memory.\n", filename); 
	fclose(file);
/*
    	int i = 0;
    	unsigned char chByte;
    	while (fread(&chByte, sizeof(unsigned char), 1, file) == 1 && i < MEMORY_SIZE) {
        	IM[i++] = chByte;
    	}	
    	fclose(file);*/
}
