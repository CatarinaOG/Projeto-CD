#ifndef DECOMPRESS_RLE
#define DECOMPRESS_RLE

#include <stdio.h>
#include <stdlib.h>
#include "SF_RLE_common.h"


/*'C' representa que passará a ler as sequencias
    de bits que representam os diversos chars
  'T' significa que irá ler o tamanho do bloco */
void mudaInputType(char *inputType);

/*função que imprime valores de rle repetidos */
void print(FILE *fp, char valor_rle, int n_rep);

/* Lê ficheiro .rle.freq e obtem os diversos tamanhos dos blocos */
int *bufferSizesRLE(FILE *fp, int *nr_blocos);

void decompressBlockRLE(FILE *fp_original, int tam_buffer_rle, char *buffer_rle);

/* Faz descompressão RLE (Cria um novo ficheiro) */
void decompressRLE(FILE *fp_rle, FILE *fp_original, int *buffer_sizes_rle, int *nr_blocos);

#endif