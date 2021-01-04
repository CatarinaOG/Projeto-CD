#include "modulo_f.h"

int encontrouArgumento(char *arg, int argc, char *argv[]){
    int i = 2;
    for(;i < argc;i++) 
        if(!strcmp(arg,argv[i]))
            return i;
    return -1;
}

int main(int argc, char *argv[]) {
    if(argc > 1){   
        /* posicao indice 1 tem de estar o pathFile */
        int index;
        if((index = encontrouArgumento("-m",argc,argv)) != -1){
            if(++index < argc){
                char *str = argv[index];
                if(!strcmp("f",str)) moduloF(argc,argv); 
            }
        }
    }
    else printf("Comando inválido! Utilize o comando \"shafa --help\" para mais informação.\n");
    return 0;
}
