#include "decompressSF.h"
#include "decompressRLE.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/********** FUNÇÕES PRINCIPAIS **********/

void initDecompressRLE(char *path_rle){
    char *path_original, *path_freq;
    removeExtensao(path_rle,&path_original,4);
    substituiExtensao(path_rle,&path_freq,".freq",0);
    FILE *fp_rle      = fopen(path_rle,"rb"),
         *fp_freq     = fopen(path_freq,"r"),
         *fp_original = fopen(path_original,"w");
    int nr_blocos = 0, *buffer_sizes_rle = bufferSizesRLE(fp_freq, &nr_blocos);
    decompressRLE(fp_rle, fp_original, buffer_sizes_rle, &nr_blocos);
    fclose(fp_rle); fclose(fp_original);
}

void decompressSF_RLE(char modo, char *path_shaf){
    char *path_new; removeExtensao(path_shaf,&path_new,5);
    char *path_cod; substituiExtensao(path_shaf,&path_cod,".cod",5);
    FILE *fp_shaf  = fopen(path_shaf,"rb"), 
         *fp_cod   = fopen(path_cod,"r"),
         *fp_new   = fopen(path_new,"wb+");
    char check_RLE = checkRLE(fp_cod);
    int  nr_blocos = nrBlocos(fp_cod);

    if(modo == '0' || check_RLE == 'N') decompressSF(fp_shaf,fp_cod,fp_new,NULL,nr_blocos);
    else{
        char *path_original; removeExtensao(path_new,&path_original,4);
        FILE *fp_original = fopen(path_original,"wb");
        decompressSF(fp_shaf,fp_cod,fp_new,fp_original,nr_blocos);
        fclose(fp_original);
    }
    fclose(fp_shaf); fclose(fp_cod); fclose(fp_new); 
}

/********** FUNÇÂO CHAMADORA **********/

void moduloD(int argc, char *argv[]){
    if(argc == 4) decompressSF_RLE('1',argv[1]);
    else{/*Caso haja mais do que 5 argumentos, os últimos sáo ignorados"*/
        if(!strcmp(argv[4],"s")) decompressSF_RLE('0',argv[1]);
        else if(!strcmp(argv[4],"r")) initDecompressRLE(argv[1]);
        else printf("Comando inválido!\n");
    }
}

int main(int argc, char *argv[]) {
    if(argc > 1){   
        clock_t startTime = clock();
        if(!strcmp(argv[2],"-m"))
            if(!strcmp(argv[3],"d")) moduloD(argc,argv);
        clock_t finishTime = clock();
        printf("Tempo de execução: %f milisegundos\n", (double)(finishTime - startTime) * 1000 / CLOCKS_PER_SEC );
    }
    else printf("Comando inválido! Utilize o comando \"shafa --help\" para mais informação.\n");
    return 0;
}