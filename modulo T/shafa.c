 #include "modulo_t.h"
 #include "../encontraArgumento.h"
 #include <stdio.h>
 #include <stdlib.h>


int main(int argc, char *argv[]) {
    if(argc > 1){
        /* posicao indice 1 tem de estar o pathFile */
        int index;
        if((index = encontrouArgumento("-m",argc,argv)) != -1){
            if(++index < argc){
                char *str = argv[index]; 
                if(!strcmp("t",str)) moduloT(argv[1]);
                else printf("comando não válido");
            }
        }
    }
    else printf("Comando inválido! Utilize o comando \"shafa --help\" para mais informação.\n");
    return 0;
}
