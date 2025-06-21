#include "parse.h"
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>

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
        return ERR_LARGE_DIGIT; // Error: Not a valid number
    }

    // Check if the address fits within 12 bits (0x000 to 0xFFF).
    if (address < 0 || address > 0xFFF) {
        return ERR_LARGE_DIGIT; // Error: Address is too large for 12 bits
    }

    return address & 0x0fff; // return just 12 bits
}

int get_special_reg(char* reg){
    // for special registers, like I, [I], ST and DT
    // Step 1: separate registers by length
    // On length 3 we check only if it's a [I]
    // On length 2 it could be ST or DT
    // On length 1 it could be I, K,  F or B (technically, they're not registers, but they used in
    // chip8 assembly language as special operands)
    // If no special reg found, return REG_ERR_UNKNOWN
    size_t length = strlen(reg);
    if(length == 3){ // It's [I]
        if(reg[0] == '[' && reg[1] == 'I' && reg[2] == ']')
            return 0x1; // means [I] special word
    }
    else if (length == 2){ // ST or DT
        if(reg[1] == 'T'){
            if(reg[0] == 'S') return 0x2; // means ST
            else if (reg[0] == 'D') return 0x3; // means DT
        }
    }
    else if (length == 1){
        if(reg[0] == 'I') return 0x4; // means I
        else if (reg[0] == 'F') return 0x5; // means F
        else if (reg[0] == 'B') return 0x6; // means B
        else if (reg[0] == 'K') return 0x7; // means K
    }

    return REG_ERR_UNKNOWN; // if no register found
}

int get_reg_id(char* reg){
    // reg is in format 'Vx', where x is one of 0-F
    if(strlen(reg) < 2)
        return REG_ERR_UNKNOWN;
    if(reg[0] != 'V')
            return REG_ERR_UNKNOWN;
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
    if(address == ERR_LARGE_DIGIT){
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
        //return REG_ERR_UNKNOWN;
        return handle_jp(reg); // BAD!!!
    }
    else if(reg_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }

    // in instruction JP V0, addr register number must be 0
    if(reg_id != 0x0){
        return ERR_MISSING_OPERAND;
    }

    // get address
    int address = convert_char_to_nnn(nnn);
    if(address == ERR_LARGE_DIGIT){
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
    if(address == ERR_LARGE_DIGIT){
        return ERR_LARGE_DIGIT;
    }

    return 0x2000 | address;
}

int handle_se(char* reg, char* kk){
    if(reg == NULL || kk == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // find out register number
    int reg_id = get_reg_id(reg);

    //check for errors
    if(reg_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    else if(reg_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }

    // there are 2 SE: one where both operand are registers, and one where the register is only first
    int kk_reg = get_reg_id(kk);
    if(kk_reg == REG_ERR_UNKNOWN){ // so, kk in not a register, it's a byte
        kk_reg = convert_char_to_nnn(kk);
        if(kk_reg == ERR_LARGE_DIGIT){
            return ERR_LARGE_DIGIT;
        }
        kk_reg = kk_reg & 0xff; // we need only 1 byte

        // return 3xkk
        return 0x3000 | (reg_id & 0xf) << 8 | kk_reg;


    } else{ // kk is a register
        
        if(kk_reg == REG_ERR_MISSING){
            return REG_ERR_MISSING;
        }

        // return 5xy0
        return 0x5000 | (reg_id & 0xf) << 8 | (kk_reg & 0xf) << 4;
    }


}

int handle_sne(char* reg, char* kk){
    if(reg == NULL || kk == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // find out register number
    int reg_id = get_reg_id(reg);

    //check for errors
    if(reg_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    else if(reg_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }

    // there are 2 SE: one where both operand are registers, and one where the register is only first
    int kk_reg = get_reg_id(kk);
    if(kk_reg == REG_ERR_UNKNOWN){ // so, kk in not a register, it's a byte
        kk_reg = convert_char_to_nnn(kk);
        if(kk_reg == ERR_LARGE_DIGIT){
            return ERR_LARGE_DIGIT;
        }
        kk_reg = kk_reg & 0xff; // we need only 1 byte

        // return 4xkk
        return 0x4000 | (reg_id & 0xf) << 8 | kk_reg;


    } else{ // kk is a register
        
        if(kk_reg == REG_ERR_MISSING){
            return REG_ERR_MISSING;
        }

        // return 9xy0
        return 0x9000 | (reg_id & 0xf) << 8 | (kk_reg & 0xf) << 4;
    }


}

int handle_ld(char* x, char* y){
    if(x == NULL || y == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    int y_id;

    if(x_id == REG_ERR_UNKNOWN){
        // so, x  is not a regular reguster, it's special
        x_id = get_special_reg(x);
        if(x_id == REG_ERR_UNKNOWN){
            return REG_ERR_UNKNOWN;
        }
        switch(x_id){
            case 0x4: // x is I, so y must be addr
                y_id = convert_char_to_nnn(y);
                if(y_id == ERR_LARGE_DIGIT){
                    return ERR_LARGE_DIGIT;
                }
                // return 0xannn
                return 0xa000 | y_id;
            case 0x3: // x is DT, so y must be regular register
                y_id = get_reg_id(y);
                if(y_id == REG_ERR_MISSING){
                    return REG_ERR_MISSING;
                } else if(y_id == REG_ERR_UNKNOWN){
                    return ERR_INVALID_OPERAND;
                } else {
                    // return 0xfx15
                    return 0xf015 | (y_id & 0xf) << 8;
                }
            case 0x2: // x is ST, so y must be regular register
                y_id = get_reg_id(y);
                if(y_id == REG_ERR_MISSING){
                    return REG_ERR_MISSING;
                } else if(y_id == REG_ERR_UNKNOWN){
                    return ERR_INVALID_OPERAND;
                } else {
                    // return 0xfx18
                    return 0xf018 | (y_id & 0xf) << 8;
                }
            case 0x5: // x is F, so y must be regular register
                y_id = get_reg_id(y);
                if(y_id == REG_ERR_MISSING){
                    return REG_ERR_MISSING;
                } else if(y_id == REG_ERR_UNKNOWN){
                    return ERR_INVALID_OPERAND;
                } else {
                    // return 0xfx29
                    return 0xf029 | (y_id & 0xf) << 8;
                }
            case 0x6: // x is B
                y_id = get_reg_id(y);
                if(y_id == REG_ERR_MISSING){
                    return REG_ERR_MISSING;
                } else if(y_id == REG_ERR_UNKNOWN){
                    return ERR_INVALID_OPERAND;
                } else {
                    // return 0xfx33
                    return 0xf033 | (y_id & 0xf) << 8;
                }
            case 0x1: // x is [I]
                y_id = get_reg_id(y);
                if(y_id == REG_ERR_MISSING){
                    return REG_ERR_MISSING;
                } else if(y_id == REG_ERR_UNKNOWN){
                    return ERR_INVALID_OPERAND;
                } else {
                    // return 0xfx55
                    return 0xf055 | (y_id & 0xf) << 8;
                }
            default:
                return ERR_INVALID_OPERAND;

        }
    } else{
        // x is a regular register
        // check second operand y
        y_id = get_reg_id(y);
        if(y_id == REG_ERR_MISSING)
            return REG_ERR_MISSING;
        if(y_id == REG_ERR_UNKNOWN){
            // so, y is byte or special operand
            y_id = get_special_reg(y);
            if(y_id == REG_ERR_UNKNOWN){ // so, y is byte
                y_id = convert_char_to_nnn(y);
                if(y_id == ERR_LARGE_DIGIT)
                    return ERR_LARGE_DIGIT;
                if(y_id > 0xff)
                    return ERR_LARGE_DIGIT;
                // return 0x6xkk
                return 0x6000 | (x_id & 0xf) << 8 | y_id;
            } else{
                if(y_id == 0x1){ // second operand is [I]
                    // return 0xfx65
                    return 0xf065 | (x_id & 0xf) << 8;
                } else if(y_id == 0x3){ // second operand is DT
                    // return 0xfx07
                    return 0xf007 | (x_id & 0xf) << 8;
                } else if (y_id == 0x7){ // second operand is K
                    // return 0xfx0a
                    return 0xf00a | (x_id & 0xf) << 8;
                } else {
                    return ERR_INVALID_OPERAND; // wrong special register
                }
            }
        } else{
            // return LD Vx, Vy, 0x8xy0
            return 0x8000 | (x_id & 0xf) << 8 | (y_id & 0xf) << 4; 
        }
    }
}

int handle_add(char* x, char* y){
    if(x == NULL || y == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    int y_id = get_reg_id(y);
    if(y_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(y_id == REG_ERR_UNKNOWN){ // y is byte
        y_id = convert_char_to_nnn(y);
        if(y_id == ERR_LARGE_DIGIT || y_id > 0xff)
            return ERR_LARGE_DIGIT;
        return 0x7000 | (x_id & 0xf) << 8 | y_id;
    }

    return 0x8004 | (x_id & 0xf) << 8 | (y_id & 0xf) << 4;

}

int handle_or(char* x, char* y){
    if(x == NULL || y == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    int y_id = get_reg_id(y);
    if(y_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(y_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }

    return 0x8001 | (x_id & 0xf) << 8 | (y_id & 0xf) << 4;
}

int handle_and(char* x, char* y){
    if(x == NULL || y == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    int y_id = get_reg_id(y);
    if(y_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(y_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }

    return 0x8002 | (x_id & 0xf) << 8 | (y_id & 0xf) << 4;
}

int handle_xor(char* x, char* y){
    if(x == NULL || y == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    int y_id = get_reg_id(y);
    if(y_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(y_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }

    return 0x8003 | (x_id & 0xf) << 8 | (y_id & 0xf) << 4;
}

int handle_sub(char* x, char* y){
    if(x == NULL || y == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    int y_id = get_reg_id(y);
    if(y_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(y_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }

    return 0x8005 | (x_id & 0xf) << 8 | (y_id & 0xf) << 4;
}

int handle_shr(char* x, char* y){
    if(x == NULL || y == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    int y_id = get_reg_id(y);
    if(y_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(y_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }

    return 0x8006 | (x_id & 0xf) << 8 | (y_id & 0xf) << 4;
}

int handle_subn(char* x, char* y){
    if(x == NULL || y == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    int y_id = get_reg_id(y);
    if(y_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(y_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }

    return 0x8007 | (x_id & 0xf) << 8 | (y_id & 0xf) << 4;
}

int handle_shl(char* x, char* y){
    if(x == NULL || y == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    int y_id = get_reg_id(y);
    if(y_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(y_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }

    return 0x800e | (x_id & 0xf) << 8 | (y_id & 0xf) << 4;
}

int handle_rnd(char* x, char* y){
    if(x == NULL || y == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    int y_id = convert_char_to_nnn(y);
    if(y_id == ERR_LARGE_DIGIT || y_id > 0xff){
        return ERR_LARGE_DIGIT;
    }

    return 0xc000 | (x_id & 0xf) << 8 | y_id;
}

int handle_drw(char* x, char* y, char* n){
    if(x == NULL || y == NULL || n == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    int y_id = get_reg_id(y);
    if(y_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(y_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }

    int n_id = convert_char_to_nnn(n);
    if(n_id == ERR_LARGE_DIGIT || n_id > 0xf){
        return ERR_LARGE_DIGIT;
    }

    return 0xd000 | (x_id & 0xf) << 8 | (y_id & 0xf) << 4 | n_id;
}

int handle_skp(char* x){
    if(x == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    return 0xe09e | (x_id & 0xf) << 8;
}

int handle_sknp(char* x){
    if(x == NULL){
        return ERR_MISSING_OPERAND; 
    }
    // check if x is a regular register
    int x_id = get_reg_id(x);
    if(x_id == REG_ERR_MISSING){
        return REG_ERR_MISSING;
    }
    if(x_id == REG_ERR_UNKNOWN){
        return REG_ERR_UNKNOWN;
    }
    return 0xe0a1 | (x_id & 0xf) << 8;
}














