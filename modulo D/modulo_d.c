#include "modulo_d.h"

/* A variável "saveBlockLength" é usada para saber quando é que deve gravar o tamanho dos blocos, por exemplo 
quando é necessário fazer ambas as descompressões não queremos guardar os valores da descompressao RLE */
char *path_final = NULL, saveBlockLength = 'Y'; 
int nr_blocos = 0, index_bloco = 0, *tam_antes = NULL, *tam_depois = NULL;
float tempo = 0;

/********** FUNÇÕES PRINCIPAIS **********/

void printModuloD(){
    int i = 0;
    printf("\nAlexandre Martins, a93315, MIEI/CD\n");
    printf("Gabriela Prata, a93288, MIEI/CD\n");
    printf("Data: 16-dez-2020\nMódulo: d (descodificação dum ficheiro shaf)\n");
    printf("Número de blocos: %d\n",nr_blocos);
    for(;i < nr_blocos; i++)
        printf("Tamanho antes/depois do ficheiro gerado (bloco %d): %d/%d\n",i,tam_antes[i],tam_depois[i]);
    printf("Tempo de execução do módulo (milissegundos): %.03f\n",tempo);
    printf("Ficheiro gerado: %s\n\n",path_final);
} 

/* modo == 'L' lê ficheiro .freq; Caso contrário o tamanho dos blocos é
definido pelo utilizador passado na variavel *buffer_sizes_rle */
int initDecompressRLE(char *path_rle, char modo, int tam_bloco_input){
    int  *buffer_sizes_rle, r;
    char *path_original;
    removeExtensao(path_rle,&path_original,4);
    FILE *lista_fps[3];
    FILE *fp_rle      = fopen(path_rle,"rb");     CheckFile(fp_rle, NULL, 0, path_rle);                lista_fps[0] = fp_rle;
    FILE *fp_original = fopen(path_original,"w"); CheckFile(fp_original, lista_fps, 1, path_original); lista_fps[1] = fp_original;

    if(modo == 'L'){
        char *path_freq; 
        substituiExtensao(path_rle,&path_freq,".freq",0);
        FILE *fp_freq = fopen(path_freq,"r"); CheckFile(fp_freq, lista_fps, 2, path_freq); lista_fps[2] = fp_freq;
        bufferSizesRLE(fp_freq,&buffer_sizes_rle);
        free(path_freq); 
        fclose(fp_freq);
    }
    else{
        CheckPointer(buffer_sizes_rle  = (int *) malloc(sizeof(int)));
        *buffer_sizes_rle = tam_bloco_input;
    }
    
    r = decompressRLE(fp_rle, fp_original, buffer_sizes_rle, modo);

    path_final = path_original;

    fclose(fp_rle); fclose(fp_original); free(buffer_sizes_rle);
    return r;
}

int decompressSF_RLE(char modo, char *path_shaf){
    int r; char *path_new, *path_cod;
    removeExtensao(path_shaf,&path_new,5);
    substituiExtensao(path_shaf,&path_cod,".cod",5);
    FILE *lista_fps[4];
    FILE *fp_shaf  = fopen(path_shaf,"rb"); CheckFile(fp_shaf, NULL, 0, path_shaf);     lista_fps[0] = fp_shaf;
    FILE *fp_cod   = fopen(path_cod,"r");   CheckFile(fp_cod, lista_fps, 1, path_cod);  lista_fps[1] = fp_cod;
    FILE *fp_new   = fopen(path_new,"wb+"); CheckFile(fp_new, lista_fps, 2, path_new);  lista_fps[2] = fp_new;
    char check_RLE = checkRLE(fp_cod);
    nr_blocos      = nrBlocos(fp_cod);

    if(modo == 'S' || check_RLE == 'N') {
        r = decompressSF(fp_shaf,fp_cod,fp_new,NULL);
        path_final = path_new;
    }
    else{
        char *path_original; removeExtensao(path_new,&path_original,4);
        FILE *fp_original = fopen(path_original,"wb"); CheckFile(fp_original, lista_fps, 3, path_original); lista_fps[3] = fp_original;
        r = decompressSF(fp_shaf,fp_cod,fp_new,fp_original);
        path_final = path_original;
        fclose(fp_original); free(path_new); 
    }

    fclose(fp_shaf); fclose(fp_cod); fclose(fp_new); free(path_cod);
    return r;
}

/********** FUNÇÂO CHAMADORA **********/

void moduloD(int argc, char *argv[]){
    int r, tam_bloco_input = 65536;/* 64 Kb (default) */
    char decompressMode    = 'A',  /* 'S' == decompressao Shannon-Fano | 'R' == decompressao RLE | 'A' == ambas as descompressoes    */
         readBlockSizeMode = 'L';  /* 'L' == ler tamanho bloco dos ficheiros .freq | 'b' usar o tamanho do bloco dado como argumento */

    if(encontrouArgumento("-r",argc,argv) != -1){
        int index; 
        decompressMode = 'R';
        if((index = encontrouArgumento("-b",argc,argv)) != -1){
            readBlockSizeMode = 'b';
            if(++index < argc){ /* Procura do argumento que indica o tamanho dos blocos, na posição seguinte  */
                char *str = argv[index]; 
                if(!strcmp("K",str))      tam_bloco_input = 655360;   /* 640Kb */
                else if(!strcmp("m",str)) tam_bloco_input = 8388608;  /* 8 Mb */
                else if(!strcmp("M",str)) tam_bloco_input = 67108864; /* 64 Mb */ 
            }
        }
    }
    else if(encontrouArgumento("-s",argc,argv) != -1) decompressMode = 'S';

    clock_t startTime = clock();
    if(decompressMode == 'R') r = initDecompressRLE(argv[1],readBlockSizeMode,tam_bloco_input);
    else r = decompressSF_RLE(decompressMode, argv[1]);
    clock_t finishTime = clock();
    tempo = (double)(finishTime - startTime) * 1000 / CLOCKS_PER_SEC;

    if(r) printModuloD();

    free(tam_antes); free(tam_depois); free(path_final);
}

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