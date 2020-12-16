#ifndef DECOMPRESS_RLE
#define DECOMPRESS_RLE

#include <stdio.h>
#include <stdlib.h>
#include "SF_RLE_common.h"

void mudaInputType(char *inputType);

void print(FILE *fp, char valor_rle, int n_rep);

int *bufferSizesRLE(FILE *fp, int *nr_blocos);

void decompressBlockRLE(FILE *fp_original, int tam_buffer_rle, char *buffer_rle);

void decompressRLE(FILE *fp_rle, FILE *fp_original, int *buffer_sizes_rle, int *nr_blocos);

#endif