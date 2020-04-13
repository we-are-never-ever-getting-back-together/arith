/* 
 * image_conversion.h
 * Assignment 4: arith
 * Authors: Zhitong Zhang (zzhang01) and Viet Nguyen (vnguye01)
 *
 * Date: March 3rd, 2020
 * 
 * This is the header file with functions which will be used to transform
 *      information of a pixel into different forms. Those functions transform 
 *      structs into different forms to be used in the compress/decompress steps
 */

#ifndef IMAGE_CONVERSION_INCLUDED
#define IMAGE_CONVERSION_INCLUDED

#include <stdio.h>
#include <stdint.h>
#include <pnm.h>
#include <math.h>

/*
 * This struct stores the red, green, blue value of a pixel after being
 *      divided by the denominator
 */
typedef struct RGB {
        float r, g, b;
} RGB;

/*
 * This struct stores the tristimulus values (Y, PR, PB) used in Component
 *      Video color space
 */
typedef struct ComponentVideo {
        float y, pb, pr;
} ComponentVideo;

/*
 * This function updates the value pointed to by the Pnm_rgb pointer with 
 *      the RGB struct and the denominator
 */
void RGB_to_Pixel (Pnm_rgb pixel, RGB rgb, unsigned denominator);

/*
 * This function returns a RGB struct based on the Pnm_rgb pixel and the 
 *      denominator
 */
RGB Pixel_to_RGB (Pnm_rgb pixel, unsigned denominator);

/*
 * This function converts the RGB form to Component Video 
 */
ComponentVideo RGB_to_ComponentVideo (RGB rgb);

/*
 * This function converts the Component Video form to RGB 
 */
RGB ComponentVideo_to_RGB (ComponentVideo cv);

/*
 * This struct stores the values that will be used for Discrete Cosine 
 *      Transformation
 */
typedef struct DCT {
        float y1, y2, y3, y4;
} DCT;

/*
 * This struct stores the values that is encoded by DCT and will be used later
 *      in bitpacking
 */
typedef struct Block {
        float a, b, c, d;
} Block;

/*
 * This function convert a Block struct into DCT form (used in decompressing)
 */
DCT Block_to_DCT (Block block);

/*
 * This function convert a DCT struct into Block form (used in compressing)
 */
Block DCT_to_Block (DCT dct);

#endif

