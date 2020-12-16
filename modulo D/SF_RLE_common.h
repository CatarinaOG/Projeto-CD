#ifndef SF_RLE_COMMON
#define SF_RLE_COMMON

#include "SF_RLE_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define skip_AtSign(fp) fseek(fp,1,SEEK_CUR); /* Dá skip ao '@' */

void removeExtensao(char *path, char **path_new, int n);

void substituiExtensao(char *path, char **path_new, char *ext_new, int n);

char checkRLE(FILE *fp);

int nrBlocos(FILE *fp);

void tamanhoBloco(FILE *fp, int *tam_bloco);

void skipNrBlocosShaf(FILE *fp);

#endif