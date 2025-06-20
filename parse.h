#pragma once

#define MAX_TOKENS 8
#define REG_ERR_UNKNOWN 0x10
#define REG_ERR_MISSING 0x11
#define ERR_UNKNOWN_MNEMONIC 0x0000
#define ERR_LARGE_DIGIT 0x0001
#define ERR_MISSING_OPERAND 0x0002

int parse_for_opcodes(char*);
