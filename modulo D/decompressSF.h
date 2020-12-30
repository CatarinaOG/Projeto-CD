#ifndef DECOMPRESS_SF
#define DECOMPRESS_SF

#include "decompressSF.h"
#include "decompressRLE.h"
#include "SF_RLE_common.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define MAX_BUFFER_COD 33151 // Pior caso: 255 chars para ';' + 1 para '\0'  + (somatório de i com i = 1 até 254) + 255*2 

typedef struct nodoCodesBTree {
  unsigned char ch; //check = '0' entao o codigo nao tem correspondencia
  struct nodoCodesBTree* zero;
  struct nodoCodesBTree* um;
} *CodesBTree;

int initCodeBTree(CodesBTree *btree);

void freeCodeBTRee(CodesBTree btree);

int analisaBloco(FILE *fp, CodesBTree* zero, CodesBTree* um);

void decompressBlockSF(unsigned char *buffer_shaf, unsigned char *buffer_new, int tam_buffer_new, CodesBTree zero, CodesBTree um);

/*Faz descompressao SF (cria ficheiro do tipo .rle ou original)
  tam_blocos_new == NULL implica que não há descompressão RLE*/
int decompressSF(FILE *fp_shaf, FILE *fp_cod, FILE *fp_new1, FILE *fp_new2);

#endif