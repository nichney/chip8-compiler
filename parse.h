#pragma once

#define MAX_TOKENS 8
#define REG_ERR_UNKNOWN -5
#define REG_ERR_MISSING -4
#define ERR_UNKNOWN_MNEMONIC -1
#define ERR_LARGE_DIGIT -2
#define ERR_MISSING_OPERAND -3
#define ERR_INVALID_OPERAND -6

int parse_for_opcodes(char*);
