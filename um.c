/*
 * Alexander Zsikla (azsikl01)
 * Partner: Ann Marie Burke (aburke04)
 * um.c
 * COMP40 HW6
 * Fall 2019
 *
 * Implementation of the full UM, which includes functions to
 * allocate/deallocate memory associated with the UM_T struct,
 * function to populate segment zero with an instruction,
 * functions to execute instructions in segment zero,
 * and functions to perform all UM instructions
 *
 */

#include <assert.h>
#include <stdio.h>
#include <stdlib.h>

#include "um.h"
#include "uarray.h"
#include "seq.h"
#include "bitpack.h"
//#include "registers.h"
#include "memory.h"

#define WORD_SIZE 32
#define OP_WIDTH 4
#define R_WIDTH 3
#define RA_LSB 6
#define RB_LSB 3
#define RC_LSB 0
#define NUM_REGISTERS 8

#define VALUE_SIZE 25
#define OP_RIGHT_SHIFT 28
#define REG_RIGHT_SHIFT 29
#define RA_LEFT_LV 4
#define RA_LEFT_SHIFT 23
#define RB_LEFT_SHIFT 26
#define RC_LEFT_SHIFT 29
#define VALUE_SHIFT 7

/* Struct definition of a UM_T which 
   contains two structs: 
   - Register_T representing the registers
   - Memory_T representing segmented memory */
struct UM_T {
    uint32_t reg[8];
    Memory_T mem;
};

/* Name: um_new
 * Input: a uint32_t representing the length of segment zero
 * Output: A newly allocated UM_T struct
 * Does: Allocates memory for a UM_T
 *       Creates a new Registers_T member and Memory_T member
 * Error: Asserts if memory is not allocated
 */
UM_T um_new(uint32_t length)
{
    UM_T um_new = malloc(sizeof(*um_new));
    assert(um_new != NULL);

    for (int i = 0; i < 8; i++) {
        um_new->reg[i] = 0;
    }
    um_new->mem = memory_new(length);

    return um_new;
}

/* Name: um_free
 * Input: A pointer to a UM_T struct 
 * Output: N/A 
 * Does: Frees all memory associated with the struct and its members
 * Error: Asserts if UM_T struct is NULL
 */
void um_free(UM_T *um)
{
    assert((*um) != NULL);

    memory_free(&(*um)->mem);
    free(*um);
}

/* Name: um_execute
 * Input: a UM_T struct
 * Output: N/A
 * Does: Executes all instructions in segment zero until
 *       there is no instruction left or until there is a halt instruction
 *       Calls instruction_call to execute all instructions except
 *       load program and load value
 * Error: Asserts if UM_T struct is NULL
 *        Asserts if segment zero is NULL at any point
 */
void um_execute(UM_T um)
{
    assert(um != NULL);

    //uint32_t *seg_zero = (uint32_t *)Seq_get(um->mem->segments, 0);
    uint32_t *seg_zero = *(um->mem->segments);
    int seg_zero_len = *seg_zero;
    int prog_counter = 0;
    uint32_t opcode, ra, rb, rc, word;

    /* Execute words in segment zero until there are none left */
    while (prog_counter < seg_zero_len) {

        if (seg_zero_len == prog_counter) {
            break;
        }

        word = *(seg_zero + 1 + prog_counter);
        opcode = word >> OP_RIGHT_SHIFT;
        prog_counter++;

        /* Load value */
        if (opcode == 13) {
            ra = (word << RA_LEFT_LV) >> REG_RIGHT_SHIFT;
            um->reg[ra] = (word << VALUE_SHIFT) >> VALUE_SHIFT;
            continue;
        } 

        ra = (word << RA_LEFT_SHIFT) >> REG_RIGHT_SHIFT;
        rb = (word << RB_LEFT_SHIFT) >> REG_RIGHT_SHIFT;
        rc = (word << RC_LEFT_SHIFT) >> REG_RIGHT_SHIFT;

        /* Load Program */
        if (opcode == 12) {
            /* Updates programs counter*/
            prog_counter = load_program(um, ra, rb, rc);
            seg_zero = *(um->mem->segments);
            seg_zero_len = *seg_zero;

        } else {
            instruction_call(um, opcode, ra, rb, rc);
        }
    }
}

/* Name: instruction_call
 * Input: UM_T struct of the um
 *        opcode of instruction to be executed
 *        unint32_t's representing registers A, B, C
 * Output: N/A
 * Does: Executes opcodes 0 to 11 (cmove to input)
 * Error: Asserts if opcode is invalid
 *        Asserts if any register number is valid
 *        Asserts if UM_T sruct is NULL
 * Notes: is called by um_execute
 */
void instruction_call(UM_T um, Um_opcode op, uint32_t ra, 
                      uint32_t rb, uint32_t rc)
{
    assert(op >= 0 && op < 14);
    assert(ra < NUM_REGISTERS && rb < NUM_REGISTERS && rc < NUM_REGISTERS);
    assert(um != NULL);

    switch (op) {
        case CMOV: 
            if (um->reg[rc] != 0) {
                um->reg[ra] = um->reg[rb];
            };  
            break;

        case SLOAD: 
            um->reg[ra] = memory_get(um->mem, um->reg[rb], um->reg[rc]);   
            break;

        case SSTORE: 
            memory_put(um->mem, um->reg[ra], um->reg[rb], um->reg[rc]);
            break;

        case ADD:                
            um->reg[ra] = um->reg[rb] + um->reg[rc];
            break;

        case MUL:          
            um->reg[ra] = um->reg[rb] * um->reg[rc]; 
            break;

        case DIV:  
            um->reg[ra] = um->reg[rb] / um->reg[rc];
            break;

        case NAND: 
            um->reg[ra] = ~(um->reg[rb] & um->reg[rc]);
            break;

        case HALT:
            um_free(&um);
            exit(EXIT_SUCCESS);
            break;

        case MAP:         
            um->reg[rb] = memory_map(um->mem, um->reg[rc]);
            break;

        case UNMAP:    
            memory_unmap(um->mem, um->reg[rc]);
            break;

        case OUT:  
            putchar(um->reg[rc]);
            break;

        case IN:
            um->reg[rc] = fgetc(stdin);
            if ((int)um->reg[rc] == EOF) {
                um->reg[rc] = ~0;
            }
            break;

        default: assert(true);
    }
}

/* Name: populate
 * Input: UM_T struct,
 *        uint32_t "index" representing offset of segment,
 *        uint32_t "word" representing word to put into
 *            segment zero at given offset
 * Output: N/A
 * Does: Populates segment zero at offset "index" with value "word"
 * Error: Asserts if the UM_T struct is NULL
 * Notes: called by driver to populate segment zero with all instructions
 */
void populate(UM_T um, uint32_t index, uint32_t word)
{
    assert(um != NULL);
    memory_put(um->mem, 0, index, word);
}

/* Name: load_program
 * Input: UM_T struct; uint32_t's reprsenting registers A, B, C
 * Output: a uint32_t representing the index that program should start at
 * Does: copies segment[rb value] into segment zero and returns rc value
 * Error: Asserts UM_T struct is NULL
 *        Asserts if any register number is valid
 */
uint32_t load_program(UM_T um, uint32_t ra, uint32_t rb, uint32_t rc)
{
    assert(um != NULL);
    assert(ra < NUM_REGISTERS && rb < NUM_REGISTERS && rc < NUM_REGISTERS);

    uint32_t rb_val = um->reg[rb];

    if (rb_val == 0) {
        return um->reg[rc];
    }
    /* Get the segment to copy */
    uint32_t *to_copy = um->mem->segments[rb_val];

    /* Creating a copy with the same specifications */
    int seg_len = *to_copy;
    uint32_t *copy = malloc((seg_len + 1) * sizeof(uint32_t));

    /* Deep copying */
    for (int i = 0; i < seg_len + 1; i++){
        *(copy + i) = *(to_copy + i);
    }

    /* Freeing segment 0 and inserting the copy */
    uint32_t *seg_zero = *(um->mem->segments);
    free(seg_zero);
    *(um->mem->segments) = copy;

    return um->reg[rc];
}
