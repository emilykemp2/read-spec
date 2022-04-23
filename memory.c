/*
 * Alexander Zsikla (azsikl01)
 * Partner: Ann Marie Burke (aburke04)
 * memory.c
 * COMP40 HW6
 * Fall 2019
 *
 * Implementation for the UM segmented memory, which includes functions for
 * allocation/deallocation of memory, mapping/unmapping segments, and
 * interacting with segments (get/put)
 *
 */

int num_segments = 0;

#include <assert.h>
#include <stdlib.h>
#include <stdio.h>
#include "memory.h"
#include "uarray.h"
#include "seq.h"

#define HINT 10

/* Name: memory_new
 * Input: a uint32_t representing the length of segment zero
 * Output: A newly allocated Memory_T struct
 * Does: * Creates a Sequence of UArray_T's, sets all segments to NULL besides
 *         segment 0, and segment 0 is initialized to be "length" long
         * Creates a Sequence of uint32_t pointers and 
           sets them to be the index of the unmapped segments
 * Error: Asserts if memory is not allocated
 */
Memory_T memory_new(uint32_t length)
{
        Memory_T m_new = malloc(sizeof(*m_new));
        assert(m_new != NULL);

        /* Creating the segments */
        m_new->segments = malloc(sizeof(uint32_t *));
        assert(m_new->segments != NULL);

        /* Creating the sequence to keep track of free segments */
        m_new->free = Seq_new(HINT);
        assert(m_new->free != NULL);

        /* Creating segment zero with proper length*/
        memory_map(m_new, length);

        return m_new;
}

/* Name: memory_free
 * Input: A pointer to a Memory_T struct 
 * Output: N/A 
 * Does: Frees all memory associated with the struct
 * Error: Asserts if struct is NULL
 */
void memory_free(Memory_T *m)
{
        assert(*m != NULL);

        /* Freeing the UArray_T segments */
        for (int seg_num = 0; seg_num < num_segments; ++seg_num) {
                
                uint32_t *aux = ((*m)->segments[seg_num]);
                
                /* If the segment is unmapped, there is nothing to free */
                if (aux == NULL) {
                        continue;
                } else {
                        free(aux);

                }
        }

        /* Freeing the uint32_t pointers */
        int free_len = Seq_length((*m)->free);
        for (int seg_num = 0; seg_num < free_len; ++seg_num) {
            uint32_t *integer = (uint32_t *)Seq_remhi((*m)->free);
            free(integer);
        }

        /* Freeing everything else */
        free((*m)->segments);
        Seq_free(&(*m)->free);
        free(*m);
}

/* Name: memory_put
 * Input: A Memory_T struct, a segment number, an offset, and a value
 * Output: N/A
 * Does: Inserts value at the specificed segment and offset
 * Error: Asserts if struct is NULL
 *        Asserts if segment is not mapped
 *        Asserts if offset is not mapped
 */
void memory_put(Memory_T m, uint32_t seg, uint32_t off, uint32_t val)
{
        uint32_t *queried_segment = *(m->segments + seg);
        *(queried_segment + off + 1) = val; 
}

/* Name: memory_get
 * Input: A Memory_T struct, a segment number, and an offset
 * Output: A uint32_t which represents the value at that segment and offset
 * Does: Gets the value at the specified segment number and offset and returns
 * Error: Asserts if struct is NULL
 *        Asserts if segment is not mapped
 *        Asserts if offset is not mapped
 */
uint32_t memory_get(Memory_T m, uint32_t seg, uint32_t off)
{
        assert(num_segments > (int)seg);
        uint32_t *queried_segment = *(m->segments + seg);
        return *(queried_segment + off + 1);
}

/* Name: memory_map
 * Input: A Memory_T struct, a segment number, and segment length
 * Output: the index of the mapped segment
 * Does: Creates a segment that is "length" long 
 *       with all of the segment's values being zero and 
 *       returns index of the mapped segment
 * Error: Asserts if struct is NULL
 *        Asserts if memory for segment is not allocated
 */
uint32_t memory_map(Memory_T m, uint32_t length)
{
        assert(m != NULL);

        uint32_t arr_length = length + 1;
        uint32_t *seg = malloc((arr_length) * sizeof(uint32_t));
        *(seg) = length;


        /* Setting values in new segment to 0 */
        for (uint32_t arr_index = 1; arr_index < arr_length; ++arr_index) {
                *(seg + arr_index) = 0;
        }

        /* Mapping a segment */
        uint32_t index = num_segments;
        if (Seq_length(m->free) == 0) {
            /* If there are no free segments, 
               put UArray_T at end of sequence */
            num_segments++;
            m->segments = realloc(m->segments, num_segments * sizeof(uint32_t *));
            *(m->segments + num_segments - 1) = seg;
        } else {
            /* If there is a free segment, 
               get the index and put the UArray_T at that index */
            uint32_t *free_seg_num = (uint32_t *)Seq_remlo(m->free);
            index = *free_seg_num;
            free(free_seg_num);
            *(m->segments + index) = seg;
        }

        return index;
}

/* Name: memory_unmap
 * Input: A Memory_T struct and a segment number
 * Output: N/A
 * Does: Unmaps a specified segment at the "seg_num" index and frees memory
 *       of the associated segment as well as adds that index back into the
 *       free segment sequence
 * Error: Asserts if struct is NULL
 *        Asserts if unmap segment 0
 *        Asserts if segment is NULL
 */
void memory_unmap(Memory_T m, uint32_t seg_num)
{
        assert(m != NULL);
        assert(seg_num != 0);

        uint32_t *unmap = *(m->segments + seg_num);
        free(unmap);

        uint32_t *free_seg = malloc(sizeof(uint32_t));
        assert(free_seg != NULL);

        *free_seg = seg_num;
        Seq_addhi(m->free, free_seg);

        *(m->segments + seg_num) = NULL;
}