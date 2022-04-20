/*
 * Alexander Zsikla (azsikl01)
 * Partner: Ann Marie Burke (aburke04)
 * registers.h
 * COMP40 HW6
 * Fall 2019
 *
 * Interface for the registers of the UM implementation 
 *
 */

#include <stdint.h>

#ifndef REGISTERS_H_
#define REGISTERS_H_

/* Pointer to a struct that contains the data structure for this module */
typedef struct Registers_T *Registers_T;

/* Creates/frees memory associated with a Registers_T */
Registers_T registers_new();
void registers_free(Registers_T *r);

/* Allows user to interact with Registers_T data */
void registers_put(Registers_T r, uint32_t num_register, uint32_t value);
uint32_t registers_get(Registers_T r, uint32_t num_register);

#endif