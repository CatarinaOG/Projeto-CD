#ifndef MODULO_D_MODULO_D_H
#define MODULO_D_MODULO_D_H
#include "modulo_d.h"
#include "decompressSF.h"
#include "decompressRLE.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/************** SHANNON-FANO ***************/

int initDecompressRLE(char *path_rle, char modo, int tam_bloco_input);

/* modo == 'S', não fazer descompressão RLE */
int decompressSF_RLE(char modo, char *path_shaf);

void moduloD(int argc, char *argv[]);

#endif //MODULO_D_MODULO_D_H