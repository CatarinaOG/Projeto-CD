#include "SF_RLE_common.h"

/********** Ficheiros **********/

void closeFiles (FILE *lista_fps[], int nr_fps){ 
    int i = 0;
    for(;i < nr_fps; i++) fclose(lista_fps[i]);
}

/********** Extensões **********/

int findPath(char *path, char **path_new, char *ext_new, int rmv, int add){
    int i;
    for(i = 0;path[i] != '\0';i++);
    i -= rmv; /* nº de chars a copiar da string path */
    CheckPointer(*path_new = (char *) malloc(i + add + 1)); /* i (nº de chars sem a extensao a remover) + add + 1 (este 1 corresponde ao espaço para o '\0')*/
    strncpy(*path_new,path,i);
    if(ext_new) strcat(*path_new,ext_new);
    return 1;
}

int checkExtensao(char *path, char *extensao, int tam_extensao){
    int i;
    for(i = 0;path[i] != '\0';i++);
    if(strcmp(&(path[i - tam_extensao]), extensao)) return 1;
    return 0;
}

/********** Gravar Tamanho **********/

extern int index_bloco;

int gravarTamanhoBloco(int tam_bloco, int **array_tam_bloco, char qual){
    if((*array_tam_bloco) == NULL){
        CheckPointer((*array_tam_bloco) = (int *) malloc(sizeof(int)));
    }
    else CheckPointer((*array_tam_bloco) = (int *) realloc((*array_tam_bloco),(index_bloco+1) * sizeof(int)));
    (*array_tam_bloco)[index_bloco] = tam_bloco;
    if(qual == 'D') index_bloco++;
    return 1;
}