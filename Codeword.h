/* 
 * Codeword.h
 * Assignment 4: arith
 * Authors: Zhitong Zhang (zzhang01) and Viet Nguyen (vnguye01)
 *
 * Date: March 3rd, 2020
 * This header file includes functions used to handle the compress and decompres
 *  information regarding to a 2 by 2 block.
 */

#ifndef CODEWORD
#define CODEWORD

#include <stdio.h>
#include <stdint.h>
#include <math.h>

#include "bitpack.h"
#include "image_conversion.h"
#include "Codeword.h"
#include <arith40.h>
#include <uarray2.h>

typedef struct Widths {
    int a, b, c, d, pr, pb;
} Widths;

extern const Widths w; 
extern const int total_bits;

void Codeword_run_compress (Block block, float pb, float pr);
void Codeword_run_decompress (uint64_t word, UArray2_T pixels, int c, int r);

#endif