/* 
 * compress40.c
 * Assignment 4: arith
 * Authors: Zhitong Zhang (zzhang01) and Viet Nguyen (vnguye01)
 *
 * Date: March 3rd, 2020
 * This program is the implementation of Codeword.h header file along with 
 * helper function
 */

#include "Codeword.h"
#include "except.h"
#include "image_conversion.h"

void Codeword_print (uint64_t word);
int64_t quantize (float f, int width);

void add_to_pixels (ComponentVideo cv, UArray2_T pixels, int c, int r);

const Widths w = {.a = 6, .b = 6, .c = 6, .d = 6, .pr = 4, .pb = 4}; 
const int total_bits = w.a + w.b + w.c + w.d + w.pr + w.pb;

/*
 * Function name: Codeword_run_compress
 * 
 * Parameters: A Block object storing luminances (Y values), 2 floats for
 *      pb and pr
 * 
 * Returns: None
 * 
 * Does: Quantize the a, b, c, d values in the Block object
 *               Convert pb and pr into 4 bits integers using the functions given in 
 *                      arith 40
 *               Pack 6 values into a 32 bit word, then print it out in big endian
 *                      order. 
 */

void Codeword_run_compress (Block block, float pb, float pr)
{
        uint64_t a_quant = block.a * (unsigned)((1U << w.a) - 1);
        int64_t  b_quant = quantize (block.b, w.b);
        int64_t  c_quant = quantize (block.c, w.c);
        int64_t  d_quant = quantize (block.d, w.d);

        unsigned pb_quant = Arith40_index_of_chroma (pb);
        unsigned pr_quant = Arith40_index_of_chroma (pr);

        uint64_t word = 0;
        int start_bit = 0;
        word = Bitpack_newu(word, w.pr, start_bit,  (uint64_t)pr_quant);
        start_bit += w.pr; 
        word = Bitpack_newu(word, w.pb, start_bit,  (uint64_t)pb_quant);
        start_bit += w.pb;
        word = Bitpack_news(word, w.d,  start_bit,  d_quant);
        start_bit += w.d;
        word = Bitpack_news(word, w.c,  start_bit, c_quant);
        start_bit += w.c;
        word = Bitpack_news(word, w.b,  start_bit, b_quant);
        start_bit += w.b;
        word = Bitpack_newu(word, w.a,  start_bit, a_quant);

        Codeword_print (word);
}

/*
 * Function name: Codeword_print 
 * 
 * Parameters: A 64 bit integer storing word information
 * 
 * Returns: None
 * 
 * Does: Print out each 8 bit in the 32 bit word in big endian order
 * 
 */

void Codeword_print (uint64_t word)
{
        for (int i = 24; i >= 0; i -= 8){
                putchar(Bitpack_getu(word, 8, i));
        }
}

/*
 * Function name: quantize
 * 
 * Parameters: The float value and the number of bits for each value
 * 
 * Returns: An integer after quantization
 * 
 * Does: Quantize the chroma float value so that it is in range 0.3 and - 0.3 
 *                      then scale it in the range from the minimum to maximum value that
 *                      can be attained with the number of bits 
 */

int64_t quantize (float f, int width)
{
        int64_t max_val = (1 << (width - 1)) - 1;
        if (f > 0.3) {
                return max_val;
        } else if (f < -0.3){
                return -max_val;
        } else {
                float scale = f / 0.3;
                return (int64_t) floor (scale * (float)max_val);
        }
}

/*
 * Function name: Codeword_run_decompress
 * 
 * Parameters: 64 bit integer for codeword, pixels array, indices of the first 
 *                              cell in the 2x2 block
 *                              
 * Returns: None
 * 
 * Does: This function performs the decompressing-related steps in a 2x2 block.
 *                      - From the word, extract the 4 values a, b, c, d and average pb and 
 *                              pr values.
 *                      - Convert the 4 values a, b, c, d back to DCT struct (DCT to pixel
 *                              space steps)
 *                      - From the 4 Y values in DCT, make 4 ComponentVideo structs
 *                      - Call add_to_pixel for each ComponentVideo struct with the 
 *                              respective indices in the 2D pixels array
 */

void Codeword_run_decompress (uint64_t word, UArray2_T pixels, int c, int r)
{
        Block block;
        int MSB = total_bits;
        MSB -= w.a;
        block.a =               (float)Bitpack_getu(word, w.a, MSB)/((1 << w.a) - 1);
        MSB -= w.b;
        block.b = 0.3 * (float)Bitpack_gets(word, w.b, MSB)/((1 << (w.b - 1)) - 1);
        MSB -= w.c;
        block.c = 0.3 * (float)Bitpack_gets(word, w.c, MSB)/((1 << (w.c - 1)) - 1);
        MSB -= w.d;
        block.d = 0.3 * (float)Bitpack_gets(word, w.d, MSB)/((1 << (w.d - 1)) - 1);

        MSB -= w.pb;
        float pb = Arith40_chroma_of_index (Bitpack_getu(word, w.pb, MSB));
        MSB -= w.pr;
        float pr = Arith40_chroma_of_index (Bitpack_getu(word, w.pr, MSB));

        DCT dct = Block_to_DCT (block);
        ComponentVideo cv1 = {.y = dct.y1, .pb = pb, .pr = pr};
        ComponentVideo cv2 = {.y = dct.y2, .pb = pb, .pr = pr};
        ComponentVideo cv3 = {.y = dct.y3, .pb = pb, .pr = pr};
        ComponentVideo cv4 = {.y = dct.y4, .pb = pb, .pr = pr};

        add_to_pixels (cv1, pixels, c, r);
        add_to_pixels (cv2, pixels, c + 1, r);
        add_to_pixels (cv3, pixels, c, r + 1);
        add_to_pixels (cv4, pixels, c + 1, r + 1);
}

/*
 * Function name: add_to_pixels
 * 
 * Parameters: A ComponentVideo struct, array of pixels, indices in the 2D array
 * 
 * Returns: None
 * 
 * Does: This function convert the ComponentVideo struct into RGB form. Then 
 *                      with the denominator of 255, it is scaled then added to the pixels 
 *                      array we passed.
 */
void add_to_pixels (ComponentVideo cv, UArray2_T pixels, int c, int r)
{
        RGB rgb = ComponentVideo_to_RGB (cv);

        Pnm_rgb aux = UArray2_at(pixels, c, r);
        RGB_to_Pixel (aux, rgb, 255);
}
