#include "load.h"
#include "memory.h"
#include <stdio.h>
#include <stddef.h>

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
