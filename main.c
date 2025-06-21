#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "utils.h"


int main(int argc, char* argv[]){
    // --- Arguments Parsing ---
    if(argc < 2){
        printf("Usage: '%s' <source_code_file>\n", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r"); // source code
    if(fp == NULL){
        printf("Error: can't open file '%s'\n", argv[1]);
        return 1;
    }

    char* bin_file = get_filename_for_binary(argv[1]);
    FILE *outp = fopen(bin_file, "wb");

    
    char line[1024]; // more that enough for one line
    int linenumber = 1;

    while(fgets(line, sizeof(line), fp) != NULL){
        int opcode = parse_for_opcode(line);

        // check for error on parsing
        switch(opcode){
            case ERR_UNKNOWN_MNEMONIC:
                printf("Error: unknown mnemonic on line %d '%s'\n", linenumber, line);
                return 1;
            case ERR_LARGE_DIGIT:
                printf("Error: too large digit on line %d '%s'\n", linenumber, line);
                return 2;
            case ERR_MISSING_OPERAND:
                printf("Error: missing operand on line %d '%s'\n", linenumber, line);
                return 3;
            case REG_ERR_UNKNOWN:
                printf("Error: unknown register number on line %d '%s'\n", linenumber, line);
                return 4;
            case REG_ERR_MISSING:
                printf("Error: missing register number on line %d '%s'\n", linenumber, line);
                return 5;
            case ERR_INVALID_OPERAND:
                printf("Error: invalid operand on line %d '%s'\n", linenumber, line);
                return 6;
        }
        linenumber++;

        fputc(((opcode & 0xff00) >> 8), outp); // writing high byte
        fputc((opcode & 0xff), outp); // writing lowest byte
    }
    

    fclose(fp);
    fclose(outp);
    free(bin_file);
    return 0;
}

