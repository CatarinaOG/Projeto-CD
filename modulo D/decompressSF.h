#ifndef DECOMPRESS_SF
#define DECOMPRESS_SF

#include "decompressSF.h"
#include "decompressRLE.h"
#include "SF_RLE_common.h"

/* Pior caso: 255 chars para ';' + 1 para '\0'  + (somatório de i com i = 1 até 254) + 255*2 */ 
#define MAX_BUFFER_COD 33151 

typedef struct nodoCodesBTree {
  unsigned char ch; 
  struct nodoCodesBTree* zero;
  struct nodoCodesBTree* um;
} *CodesBTree;

/*Aloca memória para um nodo*/ 
int initCodeBTree(CodesBTree *btree);

/*Liberta a memória alocada para árvore*/
void freeCodeBTRee(CodesBTree btree);

/*Constroi uma árvore binária com os diferentes códigos binários*/
int analisaBloco(FILE *fp, CodesBTree* zero, CodesBTree* um);

/*Analisa o bloco bit a bit, procurando na árvore binária um código compativel */
void decompressBlockSF(unsigned char *buffer_shaf, unsigned char *buffer_new, int tam_buffer_new, CodesBTree zero, CodesBTree um);

/*Faz descompressao SF (cria ficheiro do tipo .rle ou original)
 fp_new1 corresponde ao ficheiro RLE caso tenha havido esta compressao (fp_new2 corresponde ao original), 
caso contrário corresponde ao original(fp_new2 == NULL) */
int decompressSF(FILE *fp_shaf, FILE *fp_cod, FILE *fp_new1, FILE *fp_new2);

#endif