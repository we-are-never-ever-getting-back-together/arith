/* image_conversion.h
 * Assignment 4: arith
 * Authors: Zhitong Zhang (zzhang01) and Viet Nguyen (vnguye01)
 *
 * Date: March 4rd, 2020
 * 
 * This is the implementation of image_conversion header file
 */

#include "image_conversion.h"

/*
 * Function name: inRange 
 * Parameters: The value needed to put in range, max and min value of range
 * Returns: The value put in range
 * Does: Put a float value in the range we dictated by min and max value
 */
float inRange(float x, float min, float max) 
{
        if (x < min) {
                return min;
        } else if (x > max) {
                return max;
        } else {
                return x;
        }
}

/*
 *      Function name: RGB_to_Pixel
 *      Parameters: Pnm_rgb pointer to a pixel, a RGB struct, denominator value
 *      Returns: None
 *      Does: Change the values Pnm_rgb pointers pointed to based on the 
 *              denominator and RGB struct 
 */
void RGB_to_Pixel (Pnm_rgb pixel, RGB rgb, unsigned denominator)
{
        pixel->red   = (unsigned) floor(rgb.r * denominator);                           
        pixel->green = (unsigned) floor(rgb.g * denominator);
        pixel->blue  = (unsigned) floor(rgb.b * denominator);
}

/*
 * Function name: Pixel_to_RGB
 * Paramaters: Pnm_rgb pointer to a pixel, denominator value
 * Returns: A RGB struct
 * Does: From the Pnm_rgb pointer and a denominator, return a RGB struct 
 */
RGB Pixel_to_RGB (Pnm_rgb pixel, unsigned denominator)
{
        RGB rgb = {.r = (float)pixel->red /(float) denominator,
                           .g = (float)pixel->green /(float) denominator,
                           .b = (float)pixel->blue /(float) denominator};
        return rgb;
}

/*
 * Function name: RGB_to_ComponentVideo
 * Paramaters: A RGB struct
 * Returns: A ComponentVideo struct
 * Does: Comvert the value of a RGB struct into ComponentVideo format 
 *                      and return it. Values in Component Video are put in range.
 */
ComponentVideo RGB_to_ComponentVideo (RGB rgb)
{
        float y  = 0.229     * rgb.r + 0.587    * rgb.g + 0.114    * rgb.b;
        float pb = -0.168736 * rgb.r - 0.331264 * rgb.g + 0.5      * rgb.b;
        float pr = 0.5       * rgb.r - 0.418688 * rgb.g - 0.081312 * rgb.b;
        ComponentVideo cv = {.y = inRange(y, 0.0, 1.0),
                                                .pb = inRange(pb, -0.5, 0.5),
                                                .pr = inRange(pr, -0.5, 0.5)};
        return cv;
}

/*
 * Function name: ComponentVideo_to_RGB
 * Paramaters: A ComponentVideo struct
 * Returns: A RGB struct 
 * Does: Comvert the value of a ComponentVideo struct into RGB struct
 *                      and return it. Values in the RGB struct are put in range.
 */
RGB ComponentVideo_to_RGB (ComponentVideo cv)
{
        float r = 1.0 * cv.y + 0.0      * cv.pb + 1.402    * cv.pr; 
        float g = 1.0 * cv.y - 0.344136 * cv.pb - 0.714136 * cv.pr; 
        float b = 1.0 * cv.y + 1.772    * cv.pb + 0.0      * cv.pr;
        RGB rgb = {.r = inRange(r, 0.0, 1.0), 
                           .g = inRange(g, 0.0, 1.0), 
                           .b = inRange(b, 0.0, 1.0)};
        return rgb;
}

/*
 * Function name: Block_to_DCT
 * Paramters: A Block struct which contains the values a, b, c, d
 * Return: A DCT (Discrete Cosine Transformation) struct
 * Does: From the the Block struct (or in other words, pixel space),
 *                      transform back to DCT struct. Values in the DCt struct are 
 *                      put in range.
 */
DCT Block_to_DCT (Block block)
{
        float y1 = block.a - block.b - block.c + block.d;
        float y2 = block.a - block.b + block.c - block.d;
        float y3 = block.a + block.b - block.c - block.d;
        float y4 = block.a + block.b + block.c + block.d;
        DCT dct = { .y1 = inRange (y1, 0.0, 1.0), 
                                .y2 = inRange (y2, 0.0, 1.0),
                                .y3 = inRange (y3, 0.0, 1.0),
                                .y4 = inRange (y4, 0.0, 1.0)};
        return dct;
}

/*
 * Function name: DCT_to_Block
 * Paramters: A DCT (Discrete Cosine Transformation) struct
 * Return: A Block struct which contains the values a, b, c, d
 * Does: From the DCT struct, transform to Block struct (or in other words, 
 *                      pixel space). Values in the Block struct are put in range.
 */
Block DCT_to_Block (DCT dct)
{
        float a = (dct.y4 + dct.y3 + dct.y2 + dct.y1) / (4.0);
        float b = (dct.y4 + dct.y3 - dct.y2 - dct.y1) / (4.0);
        float c = (dct.y4 - dct.y3 + dct.y2 - dct.y1) / (4.0);
        float d = (dct.y4 - dct.y3 - dct.y2 + dct.y1) / (4.0);
        Block block = { .a = inRange (a,  0.0, 1.0), 
                                        .b = inRange (b, -1.0, 1.0), 
                                        .c = inRange (c, -1.0, 1.0), 
                                        .d = inRange (d, -1.0, 1.0)};
        return block;
}