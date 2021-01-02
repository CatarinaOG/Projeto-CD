#ifndef MODULO_F
#define MODULO_F

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "modulo_f.h"

typedef struct blockfreq {
	int blockSize;
	int freq[256];
	int blockSizeRLE;
	int freqRLE[256];
	struct blockfreq *next;
} *BFreq;

typedef struct freqFileBuild_output {
	int rle;
	int *arrBlock;
	int *arrBlockRLE;
} *FFBout;


// Esta funcao vai retornar 0 se correr ate ao fim sem erros.
// A funcao vai fazer a compressao RLE, escrever no ficheiro as frequencias.
// A funcao cria um ficheiro com as frequencias dos caracteres no ficheiro original quer seja realizada compressao ou nao.
// Caso seja realizaca compressao RLE entao tambem cria um ficheiro com o conteudo apos sofrer as alteracoes necessarias
//   para a compressao, e um ficheiro com as frequencias dos caracteres do mesmo.
int moduloF (int argc, char **argv);


#endif



