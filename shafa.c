#include "./modulo D/modulo_d.h"
#include "./modulo C/modulo_c.h"
#include "./modulo F/modulo_f.h"
//#include "./modulo T/modulo_t.h"
 
int main(int argc, char *argv[]) {
    if(argc > 1){   
        /* posicao indice 1 tem de estar o pathFile */
        int index;
        if((index = encontrouArgumento("-m",argc,argv)) != -1){
            if(++index < argc){
                char *str = argv[index]; 
                if(!strcmp("d",str)) moduloD(argc,argv);
                else if(!strcmp("t",str)) moduloT(argv[1]);
                else if(!strcmp("c",str)) moduloC(argc,argv);
                else if(!strcmp("f",str)) moduloF(argc,argv); 
            }
        }
    }
    else printf("Comando inválido! Utilize o comando \"shafa --help\" para mais informação.\n");
    return 0;
}