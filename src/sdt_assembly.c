#include "sdt_assembly.h"
#include "data_processing.h"
#include "tokenizer.h"
#include "assemble.h"
#include "emulate_src/decode.h"
#include <assert.h>

#define COND 14 // binary 1110 (i.e instruction always happens)
#define ASCII_0 48
#define UINT12_MAX 4095

#define ADDRESS instructions->opcodes[1]

// address - maybe char*
uint32_t sdt_assembly(tokens_t *instructions, uint16_t current_address, uint16_t *next_available_address, uint32_t *assembled_program)
{

    /*
    I = 0 -> Offset is immediate offset
    I = 1 -> Offset is shifted register

    P = 0 -> Post_indexing (offset is add/sub to base reg after transfer)
    P = 1 -> Pre_indexing (offset is add/sub to base reg before transfer)

    U = 0 -> Offset subtracted to base
    U = 1 -> Offset added from base

    L = 0 -> Store
    L = 1 -> Load

    Rn: base register

    Rd: source/destination register

    Offset: either 12 bit immediate value or a register (possibly shifted)
    */

    if (ADDRESS[0] == '=')
    {
        assert(instructions->mnemonic == LDR);

        uint32_t expression;
        expression = string_to_int(ADDRESS + 1);

        //If the expression is less than 0xFF call mov rd, #expression
        if (expression <= 0xFF)
        {
            instructions->mnemonic = MOV;
            ADDRESS[0] = '#';
            return data_process(instructions);
        }

        save_instruction(assembled_program, *next_available_address, expression);

        //Subtracted 8 for the ARM pipeline
        uint16_t offset = *next_available_address - current_address - 8;

        *next_available_address += 4;

        sprintf(instructions->opcodes[1], "[PC, #%u]", offset);

        return sdt_assembly(instructions, current_address, next_available_address, assembled_program);
    }

    uint32_t result = 0;
    enum Register_Names rd = convert_register(instructions->opcodes[0]);

    int num_bracket_opcodes = 0;
    //Remove the last ']' from <expression>
    instructions->opcodes[1][strlen(instructions->opcodes[1]) - 1] = '\0';

    //Tokenize the opcodes within []
    char **bracket_opcodes = extract_opcodes(instructions->opcodes[1] + 1, &num_bracket_opcodes);
    assert(num_bracket_opcodes);

    //Set the base register Rn
    enum Register_Names rn = convert_register(bracket_opcodes[0]);

    uint16_t offset = 0;

    //Check for pre/post index
    if (instructions->num_opcode == 2)
    {
        //Pre-Index

        //Set bit 24(P) to 1
        SET_BITS(24, 1);

        if (num_bracket_opcodes > 1)
        {
            if (bracket_opcodes[1][0] == '#')
            {
                if (bracket_opcodes[1][1] == '-' || bracket_opcodes[1][1] == '+')
                {
                    offset = string_to_int(bracket_opcodes[1] + 2);

                    //Set bit 23(U) to 1 if the number is positive
                    SET_BITS(23, bracket_opcodes[1][1] == '+');
                }
                else
                {
                    //Set bit 23(U) to 1 since number is positive
                    SET_BITS(23, 1);

                    offset = string_to_int(bracket_opcodes[1] + 1);
                }
            }
            else
            {
                //Set bit 25(I) to 1
                SET_BITS(25, 1);

                //TODO optional
            }
        }
        else
        {
            //Set bit 23(U) to 1 since number is positive
            SET_BITS(23, 1);
        }
    }
    else
    {
        if (instructions->opcodes[2][0] == '#')
        {
            if (bracket_opcodes[2][1] == '-' || bracket_opcodes[2][1] == '+')
            {
                //Set bit 23(U) to 1 if the number is positive
                SET_BITS(23, bracket_opcodes[2][1] == '+');

                offset = string_to_int(bracket_opcodes[2] + 2);
            }
            else
            {
                //Set bit 23(U) to 1 since number is positive
                SET_BITS(23, 1);

                offset = string_to_int(bracket_opcodes[2] + 1);
            }
        }
        else
        {
            //Set bit 25(I) to 1
            SET_BITS(25, 1);
            //TODO optional
        }
    }

    //Set bits 31 - 28 to Cond
    SET_BITS(28, COND);

    //Set bits 27 - 26 to 01
    SET_BITS(26, 1);

    //Set bit 25 to the I flag
    //TODO

    //Set bits 22 - 21 to 0

    //Set bit 20 to the L flag
    SET_BITS(20, instructions->mnemonic == LDR);

    //Set bits 19 - 16 to the Rn register
    SET_BITS(16, rn);

    //Set bits 15 - 12 to the Rd register
    SET_BITS(12, rd);

    //Set bits 11 - 0 to offset
    assert(offset <= UINT12_MAX);
    SET_BITS(0, offset);

    return result;
}
