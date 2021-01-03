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
// A funcao cria e escreve o ficheiro RLE, se for forçado ou se a compressão do primeiro bloco não for menor que 5%. e cria a lista de struct freqList, para otimizar 
//   a criação dos ficheiros freq e freq.RLE.
int applyRLECompression (FILE *fp_origin, BFreq *freqList, char *fileName, int checkCom, int blockSizeMultiple, float *compression);

// Esta funcao vai retornar 0 se correr ate ao fim sem erros e -2.
// A funcao cria e escreve os ficheiros freq e RLE.freq.
// Para isso, ela vai usar a lista de struct freqList e o número de blocks, criada e obtido na função "applyRLECompression".
FFBout freqFileBuild (BFreq freqList, int nblock, char *fileName);

// Esta funcao vai retornar 0 se correr ate ao fim sem erros.
// A funcao vai fazer a compressao RLE, escrever no ficheiro as frequencias.
// A funcao cria um ficheiro com as frequencias dos caracteres no ficheiro original quer seja realizada compressao ou nao.
// Caso seja realizada compressao RLE entao tambem cria um ficheiro com o conteudo apos sofrer as alteracoes necessarias
//   para a compressao, e um ficheiro com as frequencias dos caracteres do mesmo.
int moduloF (int argc, char **argv);


#endif



