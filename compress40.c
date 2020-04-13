/* 
 * compress40.c
 * Assignment 4: arith
 * Authors: Zhitong Zhang (zzhang01) and Viet Nguyen (vnguye01)
 *
 * Date: March 3rd, 2020
 * This program is the implementation of compress40.h header file
 */

#include "compress40.h"
#include <stdio.h>
#include <stdlib.h>
#include "assert.h"

#include <pnm.h>
#include <a2methods.h>
#include <a2plain.h>
#include <arith40.h>
#include <uarray2.h>
#include <bitpack.h>

#include "image_conversion.h"
#include "Codeword.h"

void compress_block (int c, int r, Pnm_ppm img);

Pnm_ppm read_header (FILE *fp);
void decompress_block (int c, int r, UArray2_T pixels, FILE *fp);

/*
 * Function name: compress40
 * 
 * Parameters: A FILE pointer
 * 
 * Returns: None
 * 
 * Does: Takes in a FILE pointer then fills the information from the picture
 *        in a Pnm_ppm object. Then, use 2 nested loops to visit the picture
 *        by 2 by 2 block, using the compress_block function.
 *         
 *       Free the Pnm_ppm object after. 
 */
void compress40 (FILE *fp){
    assert (fp != NULL);

    A2Methods_T methods = uarray2_methods_plain;
    assert(methods != NULL);

    Pnm_ppm image = Pnm_ppmread(fp, methods);
    printf("COMP40 Compressed image format 2\n%u %u\n", 
                                            image->width - image->width%2, 
                                            image->height - image->height%2);

    for (unsigned r = 0; r < image->height - image->height%2; r += 2){
        for (unsigned c = 0; c < image->width - image->width%2; c += 2){
            compress_block (c, r, image);
        }
    }
    Pnm_ppmfree(&image);
}

/*
 * Function name: compress_block
 * 
 * Parameters: col and row indices of the first pixel in the block, Pnm_ppm obj
 * 
 * Returns: None
 * 
 * Does: Convert each pixel in the vicinity of pixel with passed indices into 
 *        RGB structs. 
 *       Convert 4 RGB structs in to 4 ComponentVideo structs.
 *       From these ComponentVideo structs, calculate the average pb and pr 
 *        value. 
 *       Takes the y values of 4 ComponentVideo structs and put them 
 *        in a DCT (Discrete Cosine Transformation) struct. This struct in 
 *        converted into a Block struct, which, along with averge pb and pr
 *        value, contains information which will be used for bitpacking 
 *        (in the function Codeword_run_compress)
 */

void compress_block (int c, int r, Pnm_ppm img)
{
    float denom = img->denominator;
    RGB rgb1 = Pixel_to_RGB (img->methods->at(img->pixels, c, r),     denom);
    RGB rgb2 = Pixel_to_RGB (img->methods->at(img->pixels, c+1, r),   denom);
    RGB rgb3 = Pixel_to_RGB (img->methods->at(img->pixels, c, r+1),   denom);
    RGB rgb4 = Pixel_to_RGB (img->methods->at(img->pixels, c+1, r+1), denom);

    ComponentVideo cv1 = RGB_to_ComponentVideo(rgb1);
    ComponentVideo cv2 = RGB_to_ComponentVideo(rgb2);
    ComponentVideo cv3 = RGB_to_ComponentVideo(rgb3);
    ComponentVideo cv4 = RGB_to_ComponentVideo(rgb4);

    float pb_bar = (cv1.pb + cv2.pb + cv3.pb + cv4.pb)/4.0;
    float pr_bar = (cv1.pr + cv2.pr + cv3.pr + cv4.pr)/4.0;

    DCT dct = {.y1 = cv1.y,
               .y2 = cv2.y,
               .y3 = cv3.y,
               .y4 = cv4.y};
    Block block = DCT_to_Block (dct);
    
    Codeword_run_compress (block, pb_bar, pr_bar);
}

/*
 * Function name: compress40
 * 
 * Parameters: A FILE pointer
 * 
 * Returns: None
 * 
 * Does: - Populate the image with the read_header function
 *       - Using 2 nested for loops the compress function, we visit pixels
 *          blocks by blocks with decompress_block function. 
 *       - Print the image to stdout then free memory
 */

void decompress40 (FILE *fp)
{
    Pnm_ppm image = read_header (fp); 
    for (unsigned r = 0; r < image->height; r += 2){
        for (unsigned c = 0; c < image->width; c += 2){
            decompress_block (c, r, image->pixels, fp);
        }
    }

    Pnm_ppmwrite(stdout, image);
    Pnm_ppmfree(&image);
}

/*
 * Function name: compress40
 * 
 * Parameters: A FILE pointer
 * 
 * Returns: Pnm_ppm image pointer
 * 
 * Does: Takes in the information from the file ponter and populate the image
 *         struct with the information in the heading (width, height). Choice
 *         of denominator is 255. Allocate memory for pixels. 
 */

Pnm_ppm read_header (FILE *fp)
{
    Pnm_ppm image = malloc(sizeof(*image));; 
    A2Methods_T methods = uarray2_methods_plain;
    

    unsigned height, width;
    int read = fscanf(fp, "COMP40 Compressed image format 2\n%u %u", &width, &height); 
    assert(read == 2);
    int c = getc(fp);
    assert(c == '\n');

    
    image->width       = width;
    image->height      = height;
    image->denominator = 255;
    UArray2_T pixels   = UArray2_new (width, height, sizeof (struct Pnm_rgb));
    image->methods     = methods;
    image->pixels      = pixels;

    return image;
}

/*
 * Function name: decompress_block
 * 
 * Parameters: Indices for the starting 
 * 
 * Returns: Pnm_ppm image pointer
 * 
 * Does: Update the value of our word 8 bits (a char) at a time. After 32 bits
 *          are updated, we unpack it by using the Codeword_run_decompress 
 *          function. Codeword_run_decompress will handle the main 
 *          functionality of our decompressing (Extracting 6 values a, b, c, d,
 *          pr, pb; Convert to DCT then ComponentVideo) 
 */

void decompress_block (int c, int r, UArray2_T pixels, FILE *fp)
{
    uint64_t word = 0;
    for (int i = 24; i >= 0; i -= 8) {
        unsigned char c = getc(fp);
        word = Bitpack_newu(word, 8, i, c);
    }
    
    Codeword_run_decompress (word, pixels, c, r);
}