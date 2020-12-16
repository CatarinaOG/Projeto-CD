#include "SF_RLE_common.h"
#include "decompressRLE.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

/********** Análise do Bloco de codificações **********/

int inicializa_arr(int **tam, char **chars, char ***codes, int N){
    if( (*tam    = (int *)   malloc(sizeof(int)*N)   ) &&
        (*chars  = (char *)  malloc(sizeof(char)*N)  ) &&
        (*codes  = (char **) malloc(sizeof(char *)*N)))return 1;
    return 0;
}

void freeCodes(char **codes, int nr_codes){
    nr_codes--;
    for(;nr_codes >= 0;nr_codes--) free(codes[nr_codes]);
}

int analizaBloco(FILE *fp, int *tam, char *chars, char **codes, int *nr_codes){
    char a[256],
         ch;
    int  tam_aux = 0,  
         _ch = 0,
         i = 0; /*indice que indica o char*/

    do{ 
        ch = (char) fgetc(fp);
        if(ch == ';' || ch == '@'){
            if(tam_aux != 0){
                tam[_ch]   = tam_aux;
                chars[_ch] = (char) i;
                a[tam_aux] = '\0';
                CheckPointer(codes[_ch] = (char *) malloc(sizeof(char)*(tam_aux+1)));
                strcpy(codes[_ch],a);
                tam_aux = 0;
                _ch++;
            }
            i++;
        }
        else {
            a[tam_aux] = ch;
            tam_aux++;
        }
    }while(ch != '@');

    (*nr_codes) = _ch;

    return 1;
}

/********** Ordenação da lista de codificações **********/

void swap(void *a, void *b, size_t s){
    void *tmp = malloc(s);
    memcpy(tmp,a,s);
    memcpy(a,b,s);
    memcpy(b,tmp,s);
    free(tmp);
}

int partition (int *tam, char *chars, char **codes, int low, int high){
    int pivot = tam[high];
    int i = (low - 1);
    int j;
    for (j = low; j <= high- 1; j++){
        if (tam[j] < pivot){
            i++;
            swap(&tam[i], &tam[j], sizeof(int));
            swap(&chars[i],&chars[j], sizeof(char));
            swap(&codes[i], &codes[j], sizeof(char*));
        }
    }
    swap(&tam[i + 1], &tam[high], sizeof(int));
    swap(&chars[i + 1], &chars[high], sizeof(char));
    swap(&codes[i + 1], &codes[high], sizeof(char*));
    return (i + 1);
}

void quickSort(int *tam, char *chars, char **codes, int low, int high){
    if (low < high){
        int pi = partition(tam, chars, codes, low, high);
        quickSort(tam, chars, codes, low, pi - 1);
        quickSort(tam, chars, codes, pi + 1, high);
    }
}

/********** SHANNON-FANO **********/

void addBit(char *str,unsigned char *ch,int *shifts, int *prox_index){
    if((*ch) < 128) str[*prox_index] = '0';
    else str[*prox_index] = '1';
    (*prox_index)++;
    str[(*prox_index)] = '\0';
    (*ch) = ((*ch) << 1);
    (*shifts)++;
}

void decompressBlockSF(unsigned char *buffer_shaf, char *buffer_new, char *chars, char **codes, int nr_codes, int tam_buffer_new){
    char str[256];
    int i, _bs = 0, _bn = 0, shifts=0, prox_index=0;  /*_bn = indice buffer_new  |  _bs = indice buffer_shaf  | shifts = nª de bits analisados*/
    unsigned char ch = buffer_shaf[0];
    while(_bn < tam_buffer_new){
        if(shifts > 7){ /*Altera o char a analisar, caso os 8 bits já tenham sido verificados */
            shifts = 0;
            _bs++;
            ch = buffer_shaf[_bs];
        }
        addBit(str,&ch,&shifts,&prox_index);
        for(i=0; i<nr_codes && strcmp(str,codes[i]) ; i++);
        if(i<nr_codes){
            buffer_new[_bn] = chars[i];
            _bn++;
            prox_index = 0;
        }
    }
}

/*fp_new1 corresponde ao ficheiro RLE caso tenha havido esta compressao (fp_new2 corresponde ao original), 
caso contrário corresponde ao original(fp_new2 == NULL)*/
int decompressSF(FILE *fp_shaf, FILE *fp_cod, FILE *fp_new1, FILE *fp_new2, int nr_blocos){
    unsigned char *buffer_shaf;
    char **codes, *chars, *buffer_new;
    int *tam, i = 0, nr_codes = 0, tam_bloco_new, tam_bloco_shaf;
    inicializa_arr(&tam, &chars, &codes, 256);     /*alterar para 16 caso consiga pôr o realloc a funcionar */
    skipNrBlocosShaf(fp_shaf);

    while(i < nr_blocos){
        tamanhoBloco(fp_cod,&tam_bloco_new);
        CheckPointer(buffer_new = (char *) malloc(sizeof(char)*tam_bloco_new));

        tamanhoBloco(fp_shaf,&tam_bloco_shaf);
        CheckPointer(buffer_shaf = (unsigned char *) malloc(sizeof(unsigned char)*tam_bloco_shaf));
        fread(buffer_shaf, sizeof(unsigned char), tam_bloco_shaf, fp_shaf); skip_AtSign(fp_shaf); /* Fica a apontar para o tamanho do prox bloco */
        
        CheckReturnValue(analizaBloco(fp_cod,tam,chars,codes,&nr_codes));
        quickSort(tam,chars,codes,0,nr_codes-1);
        decompressBlockSF(buffer_shaf,buffer_new,chars,codes,nr_codes,tam_bloco_new);
        fwrite(buffer_new,sizeof(char),tam_bloco_new,fp_new1);
        if(fp_new2) decompressBlockRLE(fp_new2,tam_bloco_new,buffer_new);

        free(buffer_new); free(buffer_shaf); freeCodes(codes,nr_codes);
        nr_codes = 0; i++;
    }

    free(tam); free(chars); free(codes);
    return 1;
}