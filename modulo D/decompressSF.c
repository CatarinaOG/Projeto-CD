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
        int _buffer = 0, _char = 0; /* _char corresponde ao indice do char que se está a ler(por exemplo se _char = 0 entao o código corresponde a '\0') */
    
    /*Apenas os códigos são passados para o buffer, com a inclusão de um '\0' para marcar o final deste*/
    fscanf(fp,"%[^@]s",buffer); buffer[MAX_BUFFER_COD - 1] = '\0'; skip_AtSign(fp);

    do{ 
        ch = buffer[_buffer]; _buffer++;
        if(ch != ';' && ch != '\0'){
            /*Com a exceção dos chars especificos ';' e '\0' que indicam o final de um código, só podemos encontrar 0's e 1's.
            Usamos uma variável aux para percorrer a árvore.
            Existem duas possibilidades:
            1) aux == NULL : É atribuido o apontador de uma das árvores iniciais 
                 (Pode ser necessário inicializá-la antes de ser feita a atribuição)
            2) aux != NULL : É atribuido o apontador do nodo correspondente ao bit lido. 
                 É verificado se já existe o nodo correspondente ao bit atual, antes de ser feita a atribuição.
            */
            if(ch == '0'){
                if(aux != NULL){
                    if(!(aux->zero)) initCodeBTree(&(aux->zero));
                    aux = aux->zero;                 
                }
                else{
                    if((*zero) == NULL) initCodeBTree(zero);
                    aux = *zero;
                }
            }
            else{ //ch == '1'
                if(aux != NULL){
                    if(!(aux->um)) initCodeBTree(&(aux->um));
                    aux = aux->um;     
                }
                else{
                    if((*um) == NULL) initCodeBTree(um);
                    aux = *um; 
                }
            }
        }
        else{ /* Quando encontramos ';' ou '\0' chegamos ao final de um código, passamos entao a gravar o char a que este corresponde, no nodo do ultimo bit. */
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
        /*unsigned char ocupa 8 bits, convertendo este valor para unsigned int,
         sabemos que para um valor menor que 128 o char é do tipo 0xxxxxxx, caso contrário é do tipo 1xxxxxxx. 
         Usamos shifs para a esquerda para aceder ao bit seguinte*/
        if(ch < 128) new_bit = 0; 
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
        
        if(shifts > 7){ /*Altera o char a analisar, caso os 8 bits do char já tenham sido verificados (shifts == 8)*/
            shifts = 0;
            _bs++;
            ch = buffer_shaf[_bs];
        }
    }
}


int decompressSF(FILE *fp_shaf, FILE *fp_cod, FILE *fp_new1, FILE *fp_new2){
    unsigned char *buffer_shaf, *buffer_new;
    int i = 0, tam_bloco_new, tam_bloco_shaf;
    saveBlockLength = 'N'; /*É atribuido o valor de N para que o tamanho do bloco (guardado na variável global para posteriormente dar print) não seja mudado pelas funçoes da descompressao rle */
    fscanf(fp_shaf,"@%*d@"); /*Skip @<nº blocos>@ */
    CodesBTree zero, um; /*São criadas duas árvores, uma para cada possíbilidade de bit inicial (0 ou1)*/
    while(i < nr_blocos){

        fscanf(fp_cod,"%d@",&tam_bloco_new); 
        CheckPointer(buffer_new = (unsigned char *) malloc(sizeof(unsigned char)*tam_bloco_new));

        fscanf(fp_shaf,"%d@",&tam_bloco_shaf); 
        gravarTamanhoBloco(tam_bloco_shaf,&tam_antes,'A');
        CheckPointer(buffer_shaf = (unsigned char *) malloc(sizeof(unsigned char)*tam_bloco_shaf));
        fread(buffer_shaf, sizeof(unsigned char), tam_bloco_shaf, fp_shaf);
        skip_AtSign(fp_shaf); /* Fica a apontar para o tamanho do prox bloco */
        
        zero = um = NULL;
        CheckReturnValue(analisaBloco(fp_cod, &zero, &um));

        decompressBlockSF(buffer_shaf,buffer_new,tam_bloco_new,zero,um);
        fwrite(buffer_new,sizeof(unsigned char),tam_bloco_new,fp_new1);
        
        /* Aproveitamento do Buffer resultante da descompressão SF, para fazer a descompressao RLE */
        if(fp_new2) decompressBlockRLE(fp_new2,tam_bloco_new,buffer_new);
        else gravarTamanhoBloco(tam_bloco_new,&tam_depois,'D');

        free(buffer_new); free(buffer_shaf); freeCodeBTRee(zero); freeCodeBTRee(um);
        i++; 
    }
    return 1;
}
