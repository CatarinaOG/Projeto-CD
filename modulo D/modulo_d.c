#include "decompressSF.h"
#include "decompressRLE.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

/* A variável "saveBlockLength" é usada para saber quando é que deve gravar o tamanho dos blocos, por exemplo 
quando é necessário fazer ambas as descompressões não queremos guardar os valores da descompressao RLE */
char *path_final = NULL, saveBlockLength = 'Y'; 
int Nr_de_blocos = 0, index_bloco = 0, *tam_antes = NULL, *tam_depois = NULL;
float tempo = 0;

/********** FUNÇÕES PRINCIPAIS **********/

void printModuloD(){
    int i = 0;
    printf("\nAlexandre Martins, a93315, MIEI/CD\n");
    printf("Gabriela Prata, a93288, MIEI/CD\n");
    printf("Data: 16-dez-2020\nMódulo: d (descodificação dum ficheiro shaf)\n");
    printf("Número de blocos: %d\n",Nr_de_blocos);
    for(;i < Nr_de_blocos; i++)
        printf("Tamanho antes/depois do ficheiro gerado (bloco %d): %d/%d\n",i,tam_antes[i],tam_depois[i]);
    printf("Tempo de execução do módulo (milissegundos): %f\n",tempo);
    printf("Ficheiro gerado: %s\n\n",path_final);
} 

int initDecompressRLE(char *path_rle){
    int nr_blocos = 0, *buffer_sizes_rle, r;
    char *path_original, *path_freq;

    removeExtensao(path_rle,&path_original,4);
    substituiExtensao(path_rle,&path_freq,".freq",0);

    FILE *fp_rle      = fopen(path_rle,"rb");     CheckFile(fp_rle, path_rle);
    FILE *fp_freq     = fopen(path_freq,"r");     CheckFile(fp_freq, path_freq);
    FILE *fp_original = fopen(path_original,"w"); CheckFile(fp_original, path_original);

    bufferSizesRLE(fp_freq, &nr_blocos, &buffer_sizes_rle);
    r = decompressRLE(fp_rle, fp_original, buffer_sizes_rle, &nr_blocos);

    path_final   = path_original;
    Nr_de_blocos = nr_blocos;

    fclose(fp_rle); fclose(fp_original); free(path_freq);
    return r;
}

int decompressSF_RLE(char modo, char *path_shaf){
    int r; char *path_new, *path_cod;
    removeExtensao(path_shaf,&path_new,5);
    substituiExtensao(path_shaf,&path_cod,".cod",5);
    FILE *fp_shaf  = fopen(path_shaf,"rb"); CheckFile(fp_shaf, path_shaf);
    FILE *fp_cod   = fopen(path_cod,"r");   CheckFile(fp_cod, path_cod);
    FILE *fp_new   = fopen(path_new,"wb+"); CheckFile(fp_new, path_new);

    char check_RLE = checkRLE(fp_cod);
    int nr_blocos = nrBlocos(fp_cod);

    if(modo == '0' || check_RLE == 'N') {
        r = decompressSF(fp_shaf,fp_cod,fp_new,NULL,nr_blocos);
        path_final = path_new;
    }
    else{
        char *path_original; removeExtensao(path_new,&path_original,4);
        FILE *fp_original = fopen(path_original,"wb");
        CheckFile(fp_original, path_original);
        r = decompressSF(fp_shaf,fp_cod,fp_new,fp_original,nr_blocos);
        path_final = path_original;
        fclose(fp_original); free(path_new); 
    }

    Nr_de_blocos = nr_blocos;
    fclose(fp_shaf); fclose(fp_cod); fclose(fp_new); free(path_cod);
    return r;
}

/********** FUNÇÂO CHAMADORA **********/

void moduloD(int argc, char *argv[]){
    int r;

    clock_t startTime = clock();

    if(argc == 4) r = decompressSF_RLE('1',argv[1]);
    else{/*Caso haja mais do que 5 argumentos, os últimos sáo ignorados"*/
        if(!strcmp(argv[4],"s")) r = decompressSF_RLE('0',argv[1]);
        else if(!strcmp(argv[4],"r")) r = initDecompressRLE(argv[1]);
        else {printf("Comando inválido!\n"); return;}
    }
    
    clock_t finishTime = clock();
    tempo = (double)(finishTime - startTime) * 1000 / CLOCKS_PER_SEC;
    if(r) printModuloD();

    free(tam_antes); free(tam_depois); free(path_final);
}

int main(int argc, char *argv[]) {
    if(argc > 1){   
        if(!strcmp(argv[2],"-m"))
            if(!strcmp(argv[3],"d")) moduloD(argc,argv);
    }
    else printf("Comando inválido! Utilize o comando \"shafa --help\" para mais informação.\n");
    return 0;
}