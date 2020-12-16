#ifndef DECOMPRESS_SF
#define DECOMPRESS_SF

#include "decompressSF.h"
#include "decompressRLE.h"
#include "SF_RLE_common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

int inicializa_arr(int **tam, char **chars, char ***codes, int N);

/*Analiza o bloco, obtendo os comprimentos das diferentes sequencias de bits(array "tam"), os chars correspondentes a cada uma (array "chars")
  e as próprias sequencias (lista de strings "codes")
  fp tem de apontar para o inicio do bloco*/
int analizaBloco(FILE *fp, int *tam, char *chars, char **codes, int *nr_codes);

void swap(void *a, void *b, size_t s);

/* funçao auxiliar ao quicksort */
int partition (int *tam, char *chars, char **codes, int low, int high);

/* algoritmo quicksort obtido do site https://www.geeksforgeeks.org/quick-sort/, alterado para ordenar tanto uma lista de ints como de chars*/
void quickSort(int *tam, char *chars, char **codes, int low, int high);

/*Adiciona um bit*/
void addBit(char *str,unsigned char *aux,int *shifts, int *prox_index);

void decompressBlockSF(unsigned char *buffer_shaf, char *buffer_new, char *chars, char **codes, int nr_codes, int tam_buffer_new);

/*Faz descompressao SF (cria ficheiro do tipo .rle ou original)
  tam_blocos_new == NULL implica que não há descompressão RLE*/
int decompressSF(FILE *fp_shaf, FILE *fp_cod, FILE *fp_new1, FILE *fp_new2, int nr_blocos);

#endif