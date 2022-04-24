/*
 * Emily Kemp (ekemp01) and Phoebe Wong (pwong05)
 * um.c
 * COMP40 HW7 profiling
 * Spring 2022
 *
 * Adapted from HW6 UM code by: 
 * Alexander Zsikla (azsikl01)
 * Partner: Ann Marie Burke (aburke04)
 * memory.h
 * COMP40 HW6
 * Fall 2019
 *
 * Interface for the segmented memory of the UM implementation 
 *
 */

#include <stdint.h>
#include "seq.h"

#ifndef MEMORY_H_
#define MEMORY_H_

/* Struct definition of a Memory_T which 
   contains two sequences: 
   - one holding pointers to UArray_T's representing segments
   - one holding pointers to uint32_t's representing free segments */
struct Memory_T {
        uint32_t **segments;
        Seq_T free;
};

/* Pointer to a struct that contains the data structure for this module */
typedef struct Memory_T *Memory_T;
typedef struct Segment_T Segment_T;

/* Creates/frees memory associated with a Memory_T */
Memory_T memory_new(uint32_t length);
void memory_free(Memory_T *m);

/* Allows user to interact with Memory_T data */
void memory_put(Memory_T m, uint32_t seg, uint32_t off, uint32_t val);
uint32_t memory_get(Memory_T m, uint32_t seg, uint32_t off);

/* Maps and Unmaps segments to Memory_T sequence */
uint32_t memory_map(Memory_T m, uint32_t length);
void     memory_unmap(Memory_T m, uint32_t seg_num);

#endif