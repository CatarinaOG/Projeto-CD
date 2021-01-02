#include "modulo_d.h"

int main(int argc, char *argv[]) {
    if(argc > 1){   
        /* posicao indice 1 tem de estar o pathFile */
        int index;
        if((index = encontrouArgumento("-m",argc,argv)) != -1){
            if(++index < argc){
                char *str = argv[index]; 
                if(!strcmp("d",str)) moduloD(argc,argv);
            }
        }
    }
    else printf("Comando inválido! Utilize o comando \"shafa --help\" para mais informação.\n");
    return 0;
}