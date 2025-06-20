#include <string.h>
#include <stdio.h>

int handle_sys(char*);
int handle_jp(char*);
int handle_reg_jp(char*, char*);
int handle_call(char*);
int handle_se(char*, char*);
int handle_sne(char*, char*);
int handle_ld(char*, char*);
int handle_add(char*, char*);
int handle_or(char*, char*);
int handle_and(char*, char*);
int handle_xor(char*, char*);
int handle_sub(char*, char*);
int handle_shr(char*, char*);
int handle_subn(char*, char*);
int handle_shl(char*, char*);
int handle_rnd(char*, char*);
int handle_drw(char*, char*, char*);
int handle_skp(char*);
int handle_sknp(char*);


int parse_for_opcode(char* line){
    // Step 1: Divide string to tokens
    // Step 2: Separate mnemonics (first token in line) from operands (other tokens) 
    // [this step also include removing unneccessary characters, like ',']
    // Step 3: Switch for mnemonic and find opcode
    // Step 4: return opcode if no overloading, otherwise pass operands to recognizing functions
    // Step 5: raise error if unknown opcode is there

    int operand;

    // Split string into tokens (also removing ',')
    char* tokens[MAX_TOKENS];
    int token_count = 0;

    char* delimiters = " ,\n";
    char* token = strtok(line, delimiters);
    while (token != NULL && token_count < MAX_TOKENS) {
        tokens[token_count++] = token;
        token = strtok(NULL, delimiters);
    }

    if (token_count == 0) return ERR_UNKNOWN_MNEMONIC;

    if (strcmp(tokens[0], "SYS") == 0) {
        operand = handle_sys(tokens[1]);
    } else if (strcmp(tokens[0], "CLS") == 0) {
        operand = 0x00E0;
    } else if (strcmp(tokens[0], "RET") == 0) {
        operand = 0x00EE;
    } else if (strcmp(tokens[0], "JP") == 0) {
        if(tokens[2] != NULL){
            operand = handle_reg_jp(tokens[1], tokens[2]);
        } else {
        operand = handle_jp(tokens[1]);
        }
    } else if (strcmp(tokens[0], "CALL") == 0) {
        operand = handle_call(tokens[1]);
    } else if (strcmp(tokens[0], "SE") == 0) {
        operand = handle_se(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "SNE") == 0) {
        operand = handle_sne(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "LD") == 0) {
        operand = handle_ld(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "ADD") == 0) {
        operand = handle_add(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "OR") == 0) {
        operand = handle_or(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "AND") == 0) {
        operand = handle_and(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "XOR") == 0) {
        operand = handle_xor(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "SUB") == 0) {
        operand = handle_sub(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "SHR") == 0) {
        operand = handle_shr(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "SUBN") == 0) {
        operand = handle_subn(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "SHL") == 0) {
        operand = handle_shl(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "RND") == 0) {
        operand = handle_rnd(tokens[1], tokens[2]);
    } else if (strcmp(tokens[0], "DRW") == 0) {
        operand = handle_drw(tokens[1], tokens[2], tokens[3]);
    } else if (strcmp(tokens[0], "SKP") == 0) {
        operand = handle_skp(tokens[1]);
    } else if (strcmp(tokens[0], "SKNP") == 0) {
        operand = handle_sknp(tokens[1]);
    } else {
        return ERR_UNKNOWN_MNEMONIC;
    }

    return operand;
}


int convert_char_to_nnn(char* nnn){
    char* end;
    errno = 0; // reset errno
    
    // Base 0 allows auto-detection of the number's base (e.g., "255" or "0xFF").
    long address = strtol(nnn, &end, 0);

    // Check for various conversion errors.
    // 1. If the string is empty or contains non-numeric characters.
    // 2. If the conversion resulted in an overflow/underflow (ERANGE).
    if (end == nnn || *end != '\0' || errno == ERANGE) {
        return 0x0002; // Error: Not a valid number
    }

    // Check if the address fits within 12 bits (0x000 to 0xFFF).
    if (address < 0 || address > 0xFFF) {
        return NULL; // Error: Address is too large for 12 bits
    }

    return address & 0x0fff; // return just 12 bits
}

int get_reg_id(char* reg){
    // reg is in format 'Vx', where x is one of 0-F
    if(strlen(reg) < 2)
        printf("Error: missing register number on '%s'", reg);
        return REG_ERR_MISSING;
    switch(reg[1]){
        case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'A': return 0xa;
        case 'B': return 0xb;
        case 'C': return 0xc;
        case 'D': return 0xd;
        case 'E': return 0xe;
        case 'F': return 0xf;
        default: 
            prinf("Error: unknown register number on '%s'", reg);
            return REG_ERR_UNKNOWN;
    }
}

int handle_sys(char* nnn){
    if(nnn == NULL){
        return ERR_MISSING_OPERAND; // missing operand error
    }

    // The opcode is '0nnn', which is simply the value of the address.
    return convert_char_to_nnn(nnn);
}


int handle_jp(char* nnn){
    if(nnn == NULL){
        return ERR_MISSING_OPERAND; // missing operand error
    }
    // nnn -> int
    int address = convert_char_to_nnn(nnn);
    if(address == NULL){
        return ERR_LARGE_DIGIT;
    }
    // return 0x1nnn
    return 0x1000 | address;
    
}

int handle_reg_jp(char* reg, char* nnn){
    if(reg == NULL || nnn == NULL){
        return ERR_MISSING_OPERAND; // missing operand error
    }
    // find out register number
    int reg_id = get_reg_id(reg);

    //check for errors
    if(reg_id == REG_ERR_UNKNOWN){
        return 0x0002;
    }
    else if(reg_id == REG_ERR_MISSING){
        return 0x0002;
    }

    // in instruction JP V0, addr register number must be 0
    if(reg_id != 0x0){
        printf("Error: register must be V0");
        return 0x0002;
    }

    // get address
    int address = convert_char_to_nnn(nnn);
    if(address == NULL){
        return ERR_LARGE_DIGIT;
    }

    // return Bnnn
    return 0xb000 | address;
}

int handle_call(char* nnn){
    if(nnn == NULL){
        return ERR_MISSING_OPERAND; // missing operand error
    }
    int address = convert_char_to_nnn(nnn);    
    if(address == NULL){
        return ERR_LARGE_DIGIT;
    }

    return 0x2000 | address;
}






