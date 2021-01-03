#include "encontraArgumento.h"

/********** Ler input **********/

int encontrouArgumento(char *arg, int argc, char *argv[]){
    int i = 2;
    for(;i < argc;i++) 
        if(!strcmp(arg,argv[i]))
            return i;
    return -1;
}