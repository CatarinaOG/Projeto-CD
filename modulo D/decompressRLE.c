#include <stdio.h>
#include <stdlib.h>
#include "SF_RLE_common.h"

extern int *tam_antes;
extern int *tam_depois;
extern int index_bloco;
extern char saveBlockLength;

/********** Auxiliares **********/

void mudaInputType(char *inputType){
    if((*inputType) == 'C') (*inputType) = 'T';
    else (*inputType) = 'C';
}

void print(FILE *fp, char valor_rle, int n_rep){
    int i;
    for(i = 0;i < n_rep; i++)
        fputc(valor_rle,fp); /*(ASCII->Valor)*/
}

int bufferSizesRLE(FILE *fp, int *nr_blocos, int **buffer_sizes_rle){
    char ch, inputType = 'C';
    int _bloco = 0;
    
    fseek(fp,3,SEEK_SET); /* Skip @<R|N>@ */
    fscanf(fp,"%d",nr_blocos); 
    
    CheckPointer(*buffer_sizes_rle = (int *) malloc((*nr_blocos)*sizeof(int)));

    while(1){
        ch = (char) fgetc(fp);
        if(ch == '@'){
            mudaInputType(&inputType);
            if(inputType == 'T'){
                if((char) fgetc(fp) == '0') return 1;
                fseek(fp,-1,SEEK_CUR); /* Volta uma posição para trás, para conseguir ler o inteiro na totalidade */ 
                fscanf(fp,"%d",&((*buffer_sizes_rle)[_bloco]));
                _bloco++;
            }
            else fseek(fp,256,SEEK_CUR);
        }
    }
}

/********** RLE **********/

/* Retorna número de posições a retroceder no ficheiro */
int decompressBlockRLE(FILE *fp_original, int tam_buffer_rle, char *buffer_rle){
    char ch;
    int _buffer, nr_rep, tam_bloco_new = 0, aux;

    for(_buffer = 0; _buffer < tam_buffer_rle; _buffer++){
        if(buffer_rle[_buffer] == '\0'){
            aux = tam_buffer_rle - _buffer;
            if(aux <= 2) break;

            ch     = buffer_rle[++_buffer];
            nr_rep = (int) buffer_rle[++_buffer];
            print(fp_original,ch,nr_rep);
            tam_bloco_new += nr_rep;
        }
        else {fputc(buffer_rle[_buffer],fp_original);
              tam_bloco_new++;
        }
    }

    if(aux<=2) tam_buffer_rle -= aux;
    else aux = 0; /* aux = 0 para que não o fp_rle nao ser alterado */ 

    if(saveBlockLength == 'Y') gravarTamanhoBloco(tam_buffer_rle,&tam_antes,'A');
    gravarTamanhoBloco(tam_bloco_new,&tam_depois,'D');

    return -aux;
}

/* modo == 'L' lê ficheiro .freq;  modo == 'N' usa tamanho 
definido pelo utilizador passado na variavel *buffer_sizes_rle */
int decompressRLE(FILE *fp_rle, FILE *fp_original, int *buffer_sizes_rle, int *nr_blocos, char modo){
    int tam_bloco;
    char *buffer_rle;
    if(modo == 'L'){
        int _bloco;
        for(_bloco = 0; _bloco < (*nr_blocos); _bloco++){
            tam_bloco = buffer_sizes_rle[_bloco];
            CheckPointer(buffer_rle = (char *) malloc(sizeof(char)*tam_bloco));
            fread(buffer_rle, sizeof(char), tam_bloco, fp_rle);
            decompressBlockRLE(fp_original,tam_bloco,buffer_rle);
            free(buffer_rle);
        }
    }else{
        tam_bloco = *buffer_sizes_rle; /* Necessária para entrar no ciclo */
        CheckPointer(buffer_rle = (char *) malloc(sizeof(char)*tam_bloco));
        while(tam_bloco == *buffer_sizes_rle){
            tam_bloco = fread(buffer_rle, sizeof(char), tam_bloco, fp_rle);
            fseek(fp_rle , decompressBlockRLE(fp_original,tam_bloco,buffer_rle) , SEEK_CUR);
            (*nr_blocos)++;
        }
        free(buffer_rle);
    }
    return 1;
}