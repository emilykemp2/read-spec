/*
 * Emily Kemp (ekemp01) and Phoebe Wong (pwong05)
 * um.c
 * COMP40 HW7 profiling
 * Spring 2022
 *
 * Adapted from HW6 UM code by: 
 * Alexander Zsikla (azsikl01)
 * Partner: Ann Marie Burke (aburke04)
 * um_driver.c
 * COMP40 HW6
 * Fall 2019
 *
 * Driver file for UM Implementation.
 * The driver opens the provided .um file,
 * creates a UM_T struct,
 * reads in all instructions from given file,
 * and populates segment zero with all instructions.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <sys/stat.h>

#include "um.h"
#include "bitpack.h"

#define W_SIZE 32
#define CHAR_SIZE 8
#define CHAR_PER_WORD 4

void populate_seg_zero(UM_T um, FILE *fp, uint32_t size);
uint32_t construct_word(FILE *fp);

int main(int argc, char *argv[]) 
{
    if (argc != 2) {
        fprintf(stderr, "Usage: ./um <Um file>\n");
        return EXIT_FAILURE;
    }

    FILE *fp = fopen(argv[1], "r");

    struct stat file_info;
    stat(argv[1], &file_info);
    uint32_t size = file_info.st_size / CHAR_PER_WORD;

    UM_T um = um_new(size);

    populate_seg_zero(um, fp, size);

    fclose(fp);
    um_execute(um);

    um_free(&um);

    return EXIT_SUCCESS;
}

/* Name: populate_seg_zero
 * Input: A UM_t struct, a file pointer, and a size
 * Output: N/A
 * Does: Populates segment zero with words from the file
 */
void populate_seg_zero(UM_T um, FILE *fp, uint32_t size)
{
    populate(um, 0, size);

    for (uint32_t index = 0; index < size; ++index) {
        uint32_t word = construct_word(fp);
        populate(um, index, word);
    }
}

/* Name: construct_word
 * Input: a file pointer
 * Output: a uint32_t representing a word
 * Does: grabs 8 bits in big endian order and 
 *       creates a 32 bit word which is returned
 */
uint32_t construct_word(FILE *fp)
{
    uint32_t c = 0, word = 0;

    /* Reads in a char and creates word in big endian order */
    for (int c_loop = 0; c_loop < CHAR_PER_WORD; c_loop++) {
        c = getc(fp);
        unsigned lsb = 24 - (CHAR_SIZE * c_loop);
        word = Bitpack_newu(word, CHAR_SIZE, lsb, c);
    }

    return word;
}