#ifndef SF_RLE_COMMON
#define SF_RLE_COMMON

#include "SF_RLE_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define skip_AtSign(fp) fseek(fp,1,SEEK_CUR); /* Dá skip ao '@' */
#define skip_AtSign(fp) fseek(fp,1,SEEK_CUR); /* Dá skip ao '@' */
#define CheckPointer(x) if(!(x)) { printf("Erro: Não foi possível alocar espaço...\n"); return 0;}
#define CheckReturnValue(x) if(!(x)) return 0; /* É usada para dar return value 0 em funções dependentes do sucesso de outras */
#define CheckFile(fp,lista_fps,nr_fps,path) if(!(fp)) { closeFiles(lista_fps,nr_fps); printf("Erro: Não foi possível abrir o ficheiro (%s)...\n",path); return 0; }

int encontrouArgumento(char *arg, 
                       char argc, 
                       char *argv[]);

void closeFiles (FILE *lista_fps[], 
                 int nr_fps);

/*Remove n chars do path
  Exemplo: Se tivermos como inputs path = "aaa.txt.rle" e n = 4, temos como resultado path = "aaa.txt"*/
int removeExtensao(char *path, 
                   char **path_new, 
                   int n);

int substituiExtensao(char *path, 
                      char **path_new, 
                      char *ext_new /*nova extensão*/, 
                      int n /*tamanho da extensao anterior*/);

/*Verifica se houve compressão RLE*/
char checkRLE(FILE *fp);

/*Descobre nº de blocos em que o ficheiro original/rle foi dividido
  E deixa o fp na posicao ler o tamanho do 1º bloco */
int nrBlocos(FILE *fp);

void tamanhoBloco(FILE *fp, 
                  int *tam_bloco);

void skipNrBlocosShaf(FILE *fp);

int gravarTamanhoBloco(int tam_bloco, 
                       int **array_tam_bloco, 
                       char qual); /*qual == 'A' alteração no array tam_antes, qual=='D' alteraçao no array tam_depois */

#endif