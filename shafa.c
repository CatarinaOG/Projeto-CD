#include "./modulo D/modulo_d.h"
#include "./modulo C/thread_modulo_c.h"
#include "./modulo F/modulo_f.h"
#include "./modulo T/modulo_t.h"

int main(int argc, char *argv[]) {
    if(argc > 1){   
        /* posicao indice 1 tem de estar o pathFile */
        int index;
        if((index = encontrouArgumento("-m",argc,argv)) != -1){
            if(++index < argc){
                char *str = argv[index]; 
                if(!strcmp("d",str)) moduloD(argc,argv);
                else if(!strcmp("t",str)) moduloT(argv[1]);
                else if(!strcmp("c",str)) moduloC(argv[1]);
                else if(!strcmp("f",str)) moduloF(argc,argv); 
            }
        }
        else if(!strcmp(argv[1],"--help")){
            printf("Modulo F: -m f | opções : -b k(640kb) m(8mb) M(64mb) default 64k (tamanho dos blocos)\n");
            printf("                          -c r (força compressão rle)\n");
            printf("Modulo T: -m t\n");
            printf("Modulo C: -m c\n");
            printf("Modulo D: -m d | opções : -d s(apenas descompreção SF) r(apenas descompreção RLE)\n");
        }
    }
    else printf("Comando inválido! Utilize o comando \"shafa --help\" para mais informação.\n");
    return 0;
}
