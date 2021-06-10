#include "data_processing.h"
#include "emulate_src/data_processing.h"
#include <string.h>
#include <assert.h>
#include <stdio.h>

#define COND 14
#define UINT5_MAX 31

#define STR_TO_INT(S, I)                 \
    do                                   \
    {                                    \
        if (S[1] == '0' && S[2] == 'x')  \
        {                                \
            I = strtol(S + 3, NULL, 16); \
        }                                \
        else                             \
        {                                \
            I = strtol(S + 1, NULL, 10); \
        }                                \
    } while (0)

enum Shift_Types convert_shift_types(char *str)
{
    char *shift_types[4] = {
        "lsl",
        "lsr",
        "asr",
        "ror"};

    for (enum Shift_Types i = LSL; i <= ROR; i++)
    {
        if (strcmp(shift_types[i], str) == 0)
        {
            return i;
        }
    }

    perror("Unsupported shift type");
    exit(1);
}

uint32_t data_process(tokens *instructions)
{
    enum Register_Names rd = 0;
    enum Register_Names rn = 0;
    enum Register_Names rm = 0;
    uint8_t operand2_start;

    switch (instructions->mnemonic)
    {
    case AND:
    case EOR:
    case SUB:
    case RSB:
    case ADD:
    case ORR:
        assert(instructions->num_opcode >= 3);
        rd = convert_register(instructions->opcodes[0]);
        rn = convert_register(instructions->opcodes[1]);
        operand2_start = 2;
        break;
    case MOV:
        assert(instructions->num_opcode >= 2);
        rd = convert_register(instructions->opcodes[0]);
        operand2_start = 1;
        break;
    case TST:
    case TEQ:
    case CMP:
        assert(instructions->num_opcode >= 2);
        rn = convert_register(instructions->opcodes[0]);
        operand2_start = 1;
        break;
    default:
        perror("Unsupported instruction");
        exit(1);
        break;
    }

    //Check that the registers were read correctly
    assert(rd >= R0 && rd <= CPSR);
    assert(rn >= R0 && rn <= CPSR);

    //Set bits 31 - 28 to Cond
    uint32_t result = COND;

    //Set bits 27 - 26 to 0
    result <<= 2;

    //Set bit 25 to the I flag
    result <<= 1;
    result |= instructions->opcodes[operand2_start][0] == '#';

    //Set bits 24 - 21 to the opcode
    result <<= 4;
    result |= instructions->mnemonic - DATA_PROCESSING_INDEX;

    //Set bit 20 to the S flag
    result <<= 1;
    result |= instructions->mnemonic >= TST && instructions->mnemonic <= CMP;

    //Set bits 19 - 16 to the Rn register
    result <<= 4;
    result |= rn;

    //Set bits 15-12 to the Rd register
    result <<= 4;
    result |= rd;

    //Set bits 11 - 0 to operand2

    if (instructions->opcodes[operand2_start][0] == '#')
    {
        uint16_t immediate_result;
        STR_TO_INT(instructions->opcodes[operand2_start], immediate_result);

        assert(immediate_result <= UINT8_MAX);
        //Set the right rotation to zero and immediate value to the result
        result <<= 12;
        result |= immediate_result;
    }
    else
    {
        rm = convert_register(instructions->opcodes[operand2_start]);
        uint8_t shift = 0;
        if (instructions->num_opcode > operand2_start + 1)
        {
            enum Shift_Types shift_type = convert_shift_types(instructions->opcodes[operand2_start + 1]);
            if (instructions->opcodes[operand2_start + 2][0] == '#')
            {
                //Set bits 11 - 7 to the immediate value
                STR_TO_INT(instructions->opcodes[operand2_start + 2], shift);
                assert(shift <= UINT5_MAX);
            }
            else
            {
                //Set bits 11 - 8 to the Rs register
                enum Register_Names rs = convert_register(instructions->opcodes[operand2_start + 2]);
                assert(rs >= R0 && rs <= CPSR);
                shift = rs;

                //Set bit 7 to 0
                shift <<= 1;
            }

            //Set bits 6 - 5 to the shift type
            shift <<= 2;
            shift |= shift_type;

            //Set bit 4 to the shift type flag
            shift <<= 1;
            shift |= instructions->opcodes[operand2_start + 2][0] != '#';
        }

        //Set bits 11 - 4 to shift
        result <<= 8;
        result |= shift;

        //Set bits 3 - 0 to the Rm register
        result <<= 4;
        result |= rm;
    }

    return result;
}