/*
 * Emily Kemp (ekemp01) and Phoebe Wong (pwong05)
 * um.c
 * COMP40 HW7 profiling
 * Spring 2022
 *
 * Adapted from HW6 UM code by: 
 * Alexander Zsikla (azsikl01)
 * Partner: Ann Marie Burke (aburke04)
 * um.h
 * COMP40 HW6
 * Fall 2019
 *
 * Interface for the UM implementation
 *
 */

#include <stdint.h>

#ifndef UM_H_
#define UM_H_

/* Pointer to a struct that contains the data structure for this module */
typedef struct UM_T *UM_T;

/* Contains the indices associated with specific opcodes */
typedef enum Um_opcode {
    CMOV = 0, SLOAD, SSTORE, ADD, MUL, DIV,
    NAND, HALT, MAP, UNMAP, OUT, IN, LOADP, LV
} Um_opcode;

/* Creates/frees memory associated with a UM_T */
UM_T um_new(uint32_t length);
void um_free(UM_T *um);

/* Executes passed in program */
void um_execute(UM_T um);
void populate(UM_T um, uint32_t index, uint32_t word);

/* Instructions */
uint32_t  load_program(UM_T um, uint32_t ra, uint32_t rb, uint32_t rc);
void      load_value(UM_T um, uint32_t ra, uint32_t val);

#endif