
#ifndef _FALGS_H_EMULATOR
#define _FALGS_H_EMULATOR

#include <stdint.h>

enum Condition_Code
{
    EQ,
    NE,
    GE = 10,
    LT,
    GT,
    LE,
    AL
};

enum Flag
{
    N,
    Z,
    C,
    V
};

void set_flag_value(enum Flag flag, int8_t value, int8_t s_flag);

int check_cond(enum Condition_Code condition_code);

void set_flag(enum Flag flag);

void reset_flag(enum Flag flag);

int get_flag(enum Flag);

#endif
