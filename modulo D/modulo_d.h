#ifndef MODULO_D_MODULO_D_H
#define MODULO_D_MODULO_D_H
#include "modulo_d.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/*Verifica se houve compressão RLE*/
char checkRLE(FILE *fp);

/*Descobre nº de blocos em que o ficheiro original/rle foi dividido
  E deixa o fp na posicao ler o tamanho do 1º bloco */
int nrBlocosCod(FILE *fp);

/*Remove n chars do path
  Exemplo: Se tivermos como inputs path = "aaa.txt.rle" e n = 4, temos como resultado path = "aaa.txt"*/
void removeExtensao (char *path, char **path_new, int n);

void substituiExtensao(char *path, char **path_new, char *ext_new /*nova extensão*/, int n /*tamanho da extensao anterior*/);

/*'C' representa que passará a ler as sequencias
    de bits que representam os diversos chars
  'T' significa que irá ler o tamanho do bloco */
void mudaInputType(char *inputType);

/*função que imprime valores de rle repetidos */
void print(FILE *fp, char valor_rle, int n_rep);

/* Lê ficheiro .rle.freq e obtem os diversos tamanhos dos blocos */
int *bufferSizesRLE(FILE *fp, int *nr_blocos);

/* Faz descompressão RLE (Cria um novo ficheiro) */
void decompressRLE(FILE *fp_rle, FILE *fp_new, int *buffer_sizes_rle, int *nr_blocos);

/************** SHANNON-FANO ***************/

void inicializa_arr(int **tam, char **chars, char ***codes, int N);

void skipNrBlocosShaf(FILE *fp);

void tamanhoBloco(FILE *fp, int *tam_bloco);

/*Analiza o bloco, obtendo os comprimentos das diferentes sequencias de bits(array "tam"), os chars correspondentes a cada uma (array "chars")
  e as próprias sequencias (lista de strings "codes")
  fp tem de apontar para o inicio do bloco*/
void analizaBloco(FILE *fp, int *tam, char *chars, char **codes, int *nr_codes);

void swap(void *a, void *b, size_t s);

/* funçao auxiliar ao quicksort */
int partition (int *tam, char *chars, char **codes, int low, int high);

/* algoritmo quicksort obtido do site https://www.geeksforgeeks.org/quick-sort/, alterado para ordenar tanto uma lista de ints como de chars*/
void quickSort(int *tam, char *chars, char **codes, int low, int high);

/*Adiciona um bit*/
void addBit(char *str,unsigned char *aux,int *shifts, int *prox_index);

void auxDecompressSF(unsigned char *buffer_shaf, char *buffer_new, char *chars, char **codes, int nr_codes, int *tam_buffer_new);

/*Faz descompressao SF (cria ficheiro do tipo .rle ou original)
  tam_blocos_new == NULL implica que não há descompressão RLE*/
void decompressSF(FILE *fp_shaf, FILE *fp_cod, FILE *fp_new, int **buffer_sizes_rle, int *nr_blocos);

void initDecompressRLE(char *path_rle);

/* modo == '0', não fazer descompressão RLE */
void decompressSF_RLE(char modo, char *path_shaf);

void moduloD(int argc, char *argv[]);

#endif //MODULO_D_MODULO_D_H