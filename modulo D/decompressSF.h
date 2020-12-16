#ifndef DECOMPRESS_SF
#define DECOMPRESS_SF

#include "decompressSF.h"
#include "decompressRLE.h"
#include "SF_RLE_common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

void inicializa_arr(int **tam, char **chars, char ***codes, int N);

void swap(void *a, void *b, size_t s);

int partition (int *tam, char *chars, char **codes, int low, int high);

void quickSort(int *tam, char *chars, char **codes, int low, int high);

void analizaBloco(FILE *fp, int *tam, char *chars, char **codes, int *nr_codes);

void addBit(char *str,unsigned char *aux,int *shifts, int *prox_index);

void decompressBlockSF(unsigned char *buffer_shaf, char *buffer_new, char *chars, char **codes, int nr_codes, int tam_buffer_new);

void decompressSF(FILE *fp_shaf, FILE *fp_cod, FILE *fp_new1, FILE *fp_new2, int nr_blocos);

#endif