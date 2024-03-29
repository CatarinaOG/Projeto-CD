#ifndef DECOMPRESS_RLE
#define DECOMPRESS_RLE

#include "decompressRLE.h"
#include "SF_RLE_common.h"

/*função que imprime valores de rle repetidos */
void print_decompress_RLE(FILE *fp, char valor_rle, int n_rep);

/* Lê ficheiro .rle.freq e obtem os diversos tamanhos dos blocos */
int bufferSizesRLE(FILE *fp, int **buffer_sizes_rle);

int decompressBlockRLE(FILE *fp_original, int tam_buffer_rle, unsigned char *buffer_rle);

/* Faz descompressão RLE (Cria um novo ficheiro) */
int decompressRLE(FILE *fp_rle, FILE *fp_original, int *buffer_sizes_rle, char modo);

#endif