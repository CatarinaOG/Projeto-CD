#include "decompressSF.h"

extern int  nr_blocos;
extern int  *tam_antes;
extern int  *tam_depois;
extern int  index_bloco;
extern char saveBlockLength;

/********** Análise do Bloco de codificações **********/

int initCodeBTree(CodesBTree *btree){
    if((*btree = (CodesBTree) malloc(sizeof(struct nodoCodesBTree))) == NULL) printf("Nao deu malloc!\n");
    (*btree)->zero  = NULL;
    (*btree)->um    = NULL;
    return 1;
}

void freeCodeBTRee(CodesBTree btree){
    if(!btree) return;
    CodesBTree um = btree->um, zero = btree->zero;
    free(btree);
    freeCodeBTRee(zero);
    freeCodeBTRee(um);
}

int analisaBloco(FILE *fp, CodesBTree* zero, CodesBTree* um){
    CodesBTree aux = NULL; 
    char ch, *buffer = (char *) malloc(sizeof(char)*MAX_BUFFER_COD);
    int _buffer = 0, _char = 0; // _char corresponde ao indice do char que se está a ler(por exemplo se _char = 0 entao o código corresponde a '\0')
    fscanf(fp,"%[^@]s",buffer); buffer[MAX_BUFFER_COD - 1] = '\0'; skip_AtSign(fp);

    do{ 
        ch = buffer[_buffer]; _buffer++;
        if(ch != ';' && ch != '\0'){
            if(ch == '0'){
                if(aux == NULL){
                    if((*zero) == NULL) initCodeBTree(zero);
                    aux = *zero;                  
                }
                else{
                    if(!(aux->zero)) initCodeBTree(&(aux->zero));
                    aux = aux->zero;
                }
            }
            else{ //ch == '1'
                if(aux == NULL){
                    if((*um) == NULL) initCodeBTree(um);
                    aux = *um;       
                }
                else{
                    if(!(aux->um)) initCodeBTree(&(aux->um));
                    aux = aux->um;
                }
            }
        }
        else{
            if(aux != NULL) {
                aux->ch = _char; 
                aux = NULL;
            }
            _char++;
        }
    }while(ch != '\0');
    return 1;
}

/********** SHANNON-FANO **********/

void decompressBlockSF(unsigned char *buffer_shaf, unsigned char *buffer_new, int tam_buffer_new, CodesBTree zero, CodesBTree um){
    int _bs = 0, _bn = 0, shifts=0, new_bit;  /*_bn = indice buffer_new  |  _bs = indice buffer_shaf  | shifts = nª de bits analisados*/
    unsigned char ch = buffer_shaf[0];
    CodesBTree aux = NULL;
    while(_bn < tam_buffer_new){
        if(ch < 128) new_bit = 0; /*unsigned char ocupa 8 bits, convertendo este valor para unsigned int, sabemos que para um valor menor que 128 o char é do tipo 0xxxxxxx, caso contrário é do tipo 1xxxxxxx */
        else new_bit = 1;
        ch = ch << 1;
        shifts++;

        if(new_bit)
            aux = aux != NULL ? aux->um : um;
        else
            aux = aux != NULL ? aux->zero : zero;

        if(aux->zero == NULL && aux->um == NULL) {
            buffer_new[_bn] = aux->ch;
            _bn++;
            aux = NULL;
        }
        
        if(shifts > 7){ /*Altera o char a analisar, caso os 8 bits do char já tenham sido verificados */
            shifts = 0;
            _bs++;
            ch = buffer_shaf[_bs];
        }
    }
}

/* fp_new1 corresponde ao ficheiro RLE caso tenha havido esta compressao (fp_new2 corresponde ao original), 
caso contrário corresponde ao original(fp_new2 == NULL) */
int decompressSF(FILE *fp_shaf, FILE *fp_cod, FILE *fp_new1, FILE *fp_new2){
    unsigned char *buffer_shaf, *buffer_new;
    int i = 0, tam_bloco_new, tam_bloco_shaf;
    saveBlockLength = 'N';
    skipNrBlocosShaf(fp_shaf);
    CodesBTree zero, um;
    while(i < nr_blocos){
        tamanhoBloco(fp_cod,&tam_bloco_new);
        CheckPointer(buffer_new = (unsigned char *) malloc(sizeof(unsigned char)*tam_bloco_new));

        tamanhoBloco(fp_shaf,&tam_bloco_shaf);
        gravarTamanhoBloco(tam_bloco_shaf,&tam_antes,'A');
        CheckPointer(buffer_shaf = (unsigned char *) malloc(sizeof(unsigned char)*tam_bloco_shaf));
        fread(buffer_shaf, sizeof(unsigned char), tam_bloco_shaf, fp_shaf); skip_AtSign(fp_shaf); /* Fica a apontar para o tamanho do prox bloco */
        
        zero = um = NULL;
        CheckReturnValue(analisaBloco(fp_cod, &zero, &um));

        decompressBlockSF(buffer_shaf,buffer_new,tam_bloco_new,zero,um);
        fwrite(buffer_new,sizeof(unsigned char),tam_bloco_new,fp_new1);        
        if(fp_new2) decompressBlockRLE(fp_new2,tam_bloco_new,buffer_new);
        else gravarTamanhoBloco(tam_bloco_new,&tam_depois,'D');

        free(buffer_new); free(buffer_shaf); freeCodeBTRee(zero); freeCodeBTRee(um);
        i++; 
    }
    return 1;
}