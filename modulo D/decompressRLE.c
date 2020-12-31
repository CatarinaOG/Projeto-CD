#include "decompressRLE.h"

extern int nr_blocos;
extern int *tam_antes;
extern int *tam_depois;
extern int index_bloco;
extern char saveBlockLength;

/********** Auxiliares **********/

void print_decompress_RLE(FILE *fp, char valor_rle, int n_rep){
    int i;
    for(i = 0;i < n_rep; i++)
        fputc(valor_rle,fp); /*(ASCII->Valor)*/
}

int bufferSizesRLE(FILE *fp, int **buffer_sizes_rle){
    int _bloco = 0;
    fscanf(fp,"@%*c@%d@",&nr_blocos); /* Skip @<R|N>@  e lê o numero de blocos*/
    CheckPointer(*buffer_sizes_rle = (int *) malloc(nr_blocos*sizeof(int)));
    for(;_bloco < nr_blocos; _bloco++) {
        fscanf(fp,"%d@%*[^@]s",&((*buffer_sizes_rle)[_bloco]));
        skip_AtSign(fp);
    }
    return 1;
}

/********** RLE **********/

/* Retorna número de posições a retroceder no ficheiro.rle */
int decompressBlockRLE(FILE *fp_original, int tam_buffer_rle, unsigned char *buffer_rle){
    char ch;
    int _buffer, nr_rep, tam_bloco_new = 0, aux = tam_buffer_rle - 2; 
    /* Variavel "aux" é usada inicialmente como limite do buffer, a partir do qual não é possivel fazer a descompressao 
    do padrao que começa com '\0'. No final esta é usada para retornar o número de chars a reler para que o padrao possa 
    ser lido no próximo buffer*/

    for(_buffer = 0; _buffer < tam_buffer_rle; _buffer++){
        if(buffer_rle[_buffer] == '\0'){
            /* Se esta condicao for verdadeira, entao é guardado na variavel aux o número de "shifts" a fazer no ficheiro .rle, 
            de forma a que o padrao esteja presente no próximo buffer. Altera-se também o valor do tamanho do bloco já que 
            este vai ser escrito no terminal após o programa finalizar. */
            if(_buffer >= aux) {
                aux = tam_buffer_rle - _buffer;
                tam_buffer_rle -= aux;
                break;
            }
            ch      = buffer_rle[++_buffer];
            nr_rep  = (int) buffer_rle[++_buffer];
            print_decompress_RLE(fp_original,ch,nr_rep);
            tam_bloco_new += nr_rep;
        }
        else {
            fputc(buffer_rle[_buffer],fp_original);
            tam_bloco_new++;
        }
    }

    /*Gravacao dos tamanhos dos blocos nos arrays globais, para print no final do programa */
    if(saveBlockLength == 'Y') gravarTamanhoBloco(tam_buffer_rle,&tam_antes,'A');
    gravarTamanhoBloco(tam_bloco_new,&tam_depois,'D');

    if(aux > 2) return 0; /* aux > 2 implica que não existe necessidade de reler chars para o proximo buffer */ 
    return -aux;
}

/* modo == 'L' lê ficheiro .freq;  modo == 'N' usa tamanho 
definido pelo utilizador passado na variavel *buffer_sizes_rle */
int decompressRLE(FILE *fp_rle, FILE *fp_original, int *buffer_sizes_rle, char modo){
    int tam_bloco;
    unsigned char *buffer_rle;
    if(modo == 'L'){
        int _bloco;
        for(_bloco = 0; _bloco < nr_blocos; _bloco++){
            tam_bloco = buffer_sizes_rle[_bloco];
            CheckPointer(buffer_rle = (unsigned char *) malloc(sizeof(unsigned char)*tam_bloco));
            fread(buffer_rle, sizeof(char), tam_bloco, fp_rle);
            decompressBlockRLE(fp_original, tam_bloco, buffer_rle);
            free(buffer_rle);
        }
    }else{
        tam_bloco = *buffer_sizes_rle; /* No caso de tam_bloco diferenciar do valor dado pelo utilizador (*buffer_sizes_rle) podemos concluir que foi alcançado o último bloco */
        CheckPointer(buffer_rle = (unsigned char *) malloc(sizeof(unsigned char)*tam_bloco));
        while(tam_bloco == *buffer_sizes_rle){
            tam_bloco = fread(buffer_rle, sizeof(char), tam_bloco, fp_rle);
            fseek(fp_rle , decompressBlockRLE(fp_original,tam_bloco,buffer_rle) , SEEK_CUR);
            nr_blocos++;
        }
        free(buffer_rle);
    }
    return 1;
}