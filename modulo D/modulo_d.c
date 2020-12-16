#include "decompressSF.h"
#include "decompressRLE.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/********** FUNÇÕES PRINCIPAIS **********/

void printModuloD(int nr_blocos, int *tam_antes, int *tam_depois, float time, char *path){
    int i;
    printf("Alexandre Martins, a93315, MIEI/CD\n");
    printf("Gabriela Prata, a93288, MIEI/CD\n");
    printf("Data: 16-dez-2020\nMódulo: d (descodificação dum ficheiro shaf)\n");
    printf("Número de blocos: %d\n",nr_blocos);
    for(;i < nr_blocos;i++)
        printf("Tamanho antes/depois do ficheiro gerado (bloco %d): %d/%d",i,tam_antes[i],tam_depois[i]);
    printf("Tempo de execução do módulo (milissegundos): %f",time);
    printf("Ficheiro gerado: %s",path);
} 

void initDecompressRLE(char *path_rle){
    int nr_blocos = 0, *buffer_sizes_rle;
    char *path_original, *path_freq;
    FILE *fp_rle      = fopen(path_rle,"rb"),
         *fp_freq     = fopen(path_freq,"r"),
         *fp_original = fopen(path_original,"w");

    CheckFile(fp_rle, path_rle);
    CheckFile(fp_freq, path_freq);
    CheckFile(fp_original, path_original);

    removeExtensao(path_rle,&path_original,4);
    substituiExtensao(path_rle,&path_freq,".freq",0);

    bufferSizesRLE(fp_freq, &nr_blocos, buffer_sizes_rle);
    decompressRLE(fp_rle, fp_original, buffer_sizes_rle, &nr_blocos);

    fclose(fp_rle); fclose(fp_original); free(path_original); free(path_freq);
}

void decompressSF_RLE(char modo, char *path_shaf){
    char *path_new; removeExtensao(path_shaf,&path_new,5);
    char *path_cod; substituiExtensao(path_shaf,&path_cod,".cod",5);
    FILE *fp_shaf  = fopen(path_shaf,"rb"), 
         *fp_cod   = fopen(path_cod,"r"),
         *fp_new   = fopen(path_new,"wb+");

    CheckFile(fp_shaf, path_shaf);
    CheckFile(fp_cod, path_cod);
    CheckFile(fp_new, path_new);

    char check_RLE = checkRLE(fp_cod);
    int  nr_blocos = nrBlocos(fp_cod);

    if(modo == '0' || check_RLE == 'N') decompressSF(fp_shaf,fp_cod,fp_new,NULL,nr_blocos);
    else{
        char *path_original; removeExtensao(path_new,&path_original,4);
        FILE *fp_original = fopen(path_original,"wb");
        CheckFile(fp_original, path_original);
        decompressSF(fp_shaf,fp_cod,fp_new,fp_original,nr_blocos);
        fclose(fp_original); free(path_original);
    }

    fclose(fp_shaf); fclose(fp_cod); fclose(fp_new); free(path_new); free(path_cod);
}

/********** FUNÇÂO CHAMADORA **********/

void moduloD(int argc, char *argv[]){
    int nr_blocos, *tam_antes, *tam_depois;
    char *path_new;

    clock_t startTime = clock();
    
    if(argc == 4) decompressSF_RLE('1',argv[1]);
    else{/*Caso haja mais do que 5 argumentos, os últimos sáo ignorados"*/
        if(!strcmp(argv[4],"s")) decompressSF_RLE('0',argv[1]);
        else if(!strcmp(argv[4],"r")) initDecompressRLE(argv[1]);
        else printf("Comando inválido!\n");
    }
    
    clock_t finishTime = clock();
    double time = (double)(finishTime - startTime) * 1000 / CLOCKS_PER_SEC;
    printModuloD(nr_blocos,tam_antes,tam_depois,time,path_new);
    free(tam_antes); free(tam_depois); free(path_new);
}

int main(int argc, char *argv[]) {
    if(argc > 1){   
        if(!strcmp(argv[2],"-m"))
            if(!strcmp(argv[3],"d")) moduloD(argc,argv);
    }
    else printf("Comando inválido! Utilize o comando \"shafa --help\" para mais informação.\n");
    return 0;
}