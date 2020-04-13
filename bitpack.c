#include <inttypes.h>
#include <bitpack.h>
#include <assert.h>
#include <stdio.h>

Except_T Bitpack_Overflow = { "Overflow packing bits" };

/*
 * Function name: Bitpack_fitsu
 * Parameters: 64 bit integer, number of bits
 * Returns: Boolean value
 * Does: Check if the unsigned integer can be represented in "width" bits
 */
bool Bitpack_fitsu(uint64_t n, unsigned width){
        assert (width <= (unsigned) 64);
        if (width == (unsigned) 64){
                return true;
        } else {
                return (n >> width == (uint64_t) 0);
        }
}

/*
 * Function name: Bitpack_fitss
 * Parameters: 64 bit integer, number of bits
 * Returns: Boolean value
 * Does: Check if the signed integer can be represented in "width" bits
 */

bool Bitpack_fitss( int64_t n, unsigned width){
        assert (width <= (unsigned) 64);
        if (width == (unsigned) 0){
                return false;
        }

        int64_t one = 1;
        int64_t max_val =  (one << (width - 1));
        int64_t min_val =  ~max_val + 1;

        return (n <= max_val) && (n >= min_val);
}

/*
 * Function name: Bitpack_getu
 * Parameters: 64 bit integer, number of bits, the first significant bit
 * Returns: 64 bit unsigned integer 
 * Does: Extracts a field from a word given the width of the field and the 
 *                location of the field’s least significant bit
 */


uint64_t Bitpack_getu(uint64_t word, unsigned width, unsigned lsb){
        assert (width <= (unsigned) 64);
        assert (width + lsb <= (unsigned) 64);

        if (width == (unsigned) 0){
                return (uint64_t) 0;
        } else if (width == (unsigned)64) {
                return word;
        } else {
                uint64_t shift_left = word << (64 - width - lsb);
                return shift_left >> (64 - width);
        }
}

/*
 * Function name: Bitpack_gets
 * Parameters: 64 bit integer, number of bits, the first significant bit
 * Returns: 64 bit signed integer 
 * Does: Extracts a field from a word given the width of the field and the 
 *                location of the field’s least significant bit
 */


int64_t Bitpack_gets(uint64_t word, unsigned width, unsigned lsb){
        assert (width <= (unsigned) 64);
        assert (width + lsb <= (unsigned) 64);

        if (width == (unsigned) 0){
                return (int64_t) 0;
        } else if (width == (unsigned) 64){
                return (int64_t) word;
        } else {
                int64_t shift_left = word << (64 - width - lsb);
                return shift_left >> (64 - width);
        }
}

/*
 * Function name: Bitpack_newu
 * Parameters: 64 bit integer, number of bits, the first significant bit, 
 *                              the value we want to update
 * Returns: 64 bit unsigned integer 
 * Does: return a new word which is identical to the original word, except 
 *         that the field of width width with least significant bit at lsb
 *         will have been replaced by a width-bit representation of value
 */


uint64_t Bitpack_newu (uint64_t word, unsigned width, unsigned lsb, 
                                                    uint64_t value)
{
        assert (width <= (unsigned) 64);
        assert (width + lsb <= (unsigned) 64);
        if (!Bitpack_fitsu(value, width)){
                RAISE (Bitpack_Overflow);
        }

        uint64_t left;
        if (width == (unsigned) 0){
                return (uint64_t) value;
        } else if (width == (unsigned) 64) {
                return value;
        } else {
                left = word >> (width + lsb) << (width + lsb); 
        }

        uint64_t right;
        if (lsb == (unsigned)0){
                right = 0;
        } else {
                right = word << (64 - lsb) >> (64 - lsb);
        }

        return left | right | (value << lsb);
}

/*
 * Function name: Bitpack_news
 * Parameters: 64 bit integer, number of bits, the first significant bit, 
 *                              the value we want to update
 * Returns: 64 bit unsigned integer 
 * Does: return a new word which is identical to the original word, except 
 *         that the field of width width with least significant bit at 
 *         will have been replaced by a width-bit representation of 
 *         value
 */

uint64_t Bitpack_news(uint64_t word, unsigned width, unsigned lsb, 
                                                        int64_t value)
{
        assert (width <= (unsigned) 64);
        assert (width + lsb <= (unsigned) 64);
        if (!Bitpack_fitss(value, width)){
                RAISE (Bitpack_Overflow);
        }

        uint64_t value_u = Bitpack_getu(value, width, 0);
        return Bitpack_newu(word, width, lsb, value_u);
}