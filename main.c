#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "utils.h"


int main(int argc, char* argv[]){
    // --- Arguments Parsing ---
    if(argc < 2){
        printf("Usage: '%s' <source_code_file>", argv[0]);
        return 1;
    }

    FILE *fp = fopen(argv[1], "r"); // source code
    if(fp == NULL){
        printf("Error: can't open file '%s'\n", argv[1]);
        return 1;
    }

    char* bin_file = get_filename_for_binary(argv[1]);
    printf("bin_file = '%s'\n", bin_file);
    //FILE *outp = fopen(bin_file, "wb");

    /*
    char line[1024]; // more that enougth for one line

    while(fgets(line, sizeof(line), fp) != NULL){
        // TODO: pass the line to lezical parser
    }
    */

    fclose(fp);
    free(bin_file);
    return 0;
}

