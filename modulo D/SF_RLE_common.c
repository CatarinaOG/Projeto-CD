#include "SF_RLE_common.h"

/********** Ler input **********/

int encontrouArgumento(char *arg, char argc, char *argv[]){
    int i = 2;
    for(;i < argc;i++) 
        if(!strcmp(arg,argv[i]))
            return i;
    return -1;
}

/********** Ficheiros **********/

void closeFiles (FILE *lista_fps[], int nr_fps){ 
    int i = 0;
    for(;i < nr_fps; i++) fclose(lista_fps[i]);
}

/********** Extensões **********/

int removeExtensao(char *path, char **path_new, int n){
    int i;
    for(i = 0;path[i] != '\0';i++);
    i -= n; /* nº de chars a copiar da string path */
    CheckPointer(*path_new = (char *) malloc(i + 1));
    strncpy(*path_new,path,i);
    return 1;
}

int substituiExtensao(char *path, char **path_new, char *ext_new, int n){
    CheckReturnValue(removeExtensao(path,path_new,n));
    strcat(*path_new,ext_new);
    return 1;
}

int checkExtensao(char *path, char *extensao, int tam_extensao){
    int i;
    for(i = 0;path[i] != '\0';i++);
    if(strcmp(&(path[i - tam_extensao]), extensao)) return 1;
    return 0;
}

/********** Ler ficheiros **********/

char checkRLE(FILE *fp){
    skip_AtSign(fp);
    return (char) fgetc(fp);
}

int nrBlocos(FILE *fp){
    int Nr_blocos;
    skip_AtSign(fp);
    fscanf(fp,"%d",&Nr_blocos);
    skip_AtSign(fp);
    return Nr_blocos;
}

void tamanhoBloco(FILE *fp, int *tam_bloco){
    fscanf(fp,"%d",tam_bloco);
    skip_AtSign(fp);
}

void skipNrBlocosShaf(FILE *fp){
    skip_AtSign(fp);
    fscanf(fp,"%*d"); /*Skip ao nº de blocos*/
    skip_AtSign(fp);
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