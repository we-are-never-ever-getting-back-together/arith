#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include <pnm.h>
#include <a2methods.h>
#include <a2plain.h>
#include <uarray2.h>
#include "assert.h"


int main(int argc, char *argv[]){
        (void) argc;
        FILE *fp1 = fopen(argv[1], "r");
        FILE *fp2 = fopen(argv[2], "r");
        assert (fp1 != NULL);
        assert (fp2 != NULL);

    A2Methods_T methods = uarray2_methods_plain;
        assert(methods != NULL);

        Pnm_ppm image1 = Pnm_ppmread(fp1, methods);
        Pnm_ppm image2 = Pnm_ppmread(fp2, methods);

        assert (abs((int) image2->width  - (int) image1->width) <= 1);
        assert (abs((int) image2->height - (int) image1->height) <= 1);

        unsigned width = image2->width > image1->width ? image1->width : image2->width;
        unsigned height = image2->height > image1->height ? image1->height : image2->height;

        float f = 0.0;
        for (unsigned r = 0; r < height; r += 2){
        for (unsigned c = 0; c < width; c += 2){
                float denom1 = image1->denominator;
                float denom2 = image2->denominator;
                Pnm_rgb pixel1 = image1->methods->at(image1->pixels, c, r);
                Pnm_rgb pixel2 = image2->methods->at(image2->pixels, c, r);

                f += (float) (pixel1->red/denom1 - pixel2->red/denom2) 
                                        * (pixel1->red/denom1 - pixel2->red/denom2);
                f += (float) (pixel1->blue/denom1 - pixel2->blue/denom2) 
                                        * (pixel1->blue/denom1 - pixel2->blue/denom2);
                f += (float) (pixel1->green/denom1 - pixel2->green/denom2) 
                                        * (pixel1->green/denom1 - pixel2->green/denom2);
        }
    }
    f /= (float) (3 * width * height);
    f = sqrt(f);
    printf ("%0.8f \n", f);
}