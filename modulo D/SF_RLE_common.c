#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "SF_RLE_common.h"

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


/********** Ler ficheiros **********/

char checkRLE(FILE *fp){
    skip_AtSign(fp);
    return (char) fgetc(fp);
}

int nrBlocos(FILE *fp){
    int nr_blocos;
    skip_AtSign(fp);
    fscanf(fp,"%d",&nr_blocos);
    skip_AtSign(fp);
    return nr_blocos;
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