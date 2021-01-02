#ifndef SF_RLE_COMMON
#define SF_RLE_COMMON

#include "SF_RLE_common.h"
#include "../encontraArgumento.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define skip_AtSign(fp) fseek(fp,1,SEEK_CUR); /* Dá skip ao '@' */
#define CheckPointer(x) if(!(x)) { printf("Erro: Não foi possível alocar espaço...\n"); return 0;}
#define CheckReturnValue(x) if(!(x)) return 0; /* É usada para dar return value 0 em funções dependentes do sucesso de outras */
#define CheckFile(fp,lista_fps,nr_fps,path) if(!(fp)) { closeFiles(lista_fps,nr_fps); printf("Erro: Não foi possível abrir o ficheiro (%s)...\n",path); return 0; }

void closeFiles (FILE *lista_fps[], 
                 int nr_fps);

int findPath(char *path, 
               char **path_new,
               char *ext_new, 
               int rmv, /* chars a remover (tamanho da extensao que se pretende remover) */
               int add); /* chars a adicionar (tamanho da extensao que se pretende adicionar)*/

int gravarTamanhoBloco(int tam_bloco, 
                       int **array_tam_bloco, 
                       char qual); /*qual == 'A' alteração no array tam_antes, qual=='D' alteraçao no array tam_depois */

#endif