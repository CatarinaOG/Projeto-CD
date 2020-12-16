#include <stdio.h>
#include <stdlib.h>
#include "SF_RLE_common.h"

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

int bufferSizesRLE(FILE *fp, int *nr_blocos, int *buffer_sizes_rle){
    char ch, inputType = 'C';
    int _bloco = 0;
    
    fseek(fp,3,SEEK_SET); /* Skip @<R|N>@ */
    fscanf(fp,"%d",nr_blocos); 
    
    CheckPointer(buffer_sizes_rle = (int *) malloc((*nr_blocos)*sizeof(int)));

    while(1){
        ch = (char) fgetc(fp);
        if(ch == '@'){
            mudaInputType(&inputType);
            if(inputType == 'T'){
                if((char) fgetc(fp) == '0') return 1;
                fseek(fp,-1,SEEK_CUR); /* Volta uma posição para trás, para conseguir ler o inteiro na totalidade */ 
                fscanf(fp,"%d",&(buffer_sizes_rle[_bloco]));
                _bloco++;
            }
            else fseek(fp,256,SEEK_CUR);
        }
    }
}

/********** RLE **********/

void decompressBlockRLE(FILE *fp_original, int tam_buffer_rle, char *buffer_rle){
    char ch;
    int _buffer, nr_rep;
    for(_buffer = 0; _buffer < tam_buffer_rle ; _buffer++){
            if(buffer_rle[_buffer] == '\0'){
                ch     = buffer_rle[++_buffer];
                nr_rep = (int) buffer_rle[++_buffer];
                print(fp_original,ch,nr_rep);
            }
            else fputc(buffer_rle[_buffer],fp_original);
    }
}

int decompressRLE(FILE *fp_rle, FILE *fp_original, int *buffer_sizes_rle, int *nr_blocos){
    int _bloco,  tam_bloco;
    char *buffer_rle;
    for(_bloco = 0; _bloco < (*nr_blocos); _bloco++){
        tam_bloco = buffer_sizes_rle[_bloco];
        CheckPointer(buffer_rle = (char *) malloc(sizeof(char)*tam_bloco));
        fread(buffer_rle, sizeof(char), tam_bloco, fp_rle);
        decompressBlockRLE(fp_original,tam_bloco,buffer_rle);
        free(buffer_rle);
    }
    return 1;
}