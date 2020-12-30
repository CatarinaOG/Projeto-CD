#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//-----------------------------------------------------------------LEITURA-------------------------------------------------------------------------

// Descobre se ocorreu RLE ou nao
char ORIG_RLE (FILE *fp){

    char orig_rle;
    fscanf(fp,"@%c@",&orig_rle);

return orig_rle;
}

// Recolhe o nr de blocos
int descobre_nr_blocos(FILE *fp){

    int nrBlocos;
    fscanf(fp,"%d",&nrBlocos);

return nrBlocos;
}

typedef struct simbolo {
    int valor; // valor do simbolo
    int freq; // frequencia do simbolo
    int *bits; // sequencia de bits shanon-fano
    int fp; // guarda o indice do ultimo bit preenchido
}*Nodo;

void copiaFreqAnterior (Nodo listaStruct , int i){

    listaStruct[i].valor = i;
    listaStruct[i].freq = listaStruct[i-1].freq;
    (listaStruct[i]).bits = (int*)malloc(sizeof(int)*256);
    (listaStruct[i]).fp = -1;

}

// converte 1 bloco do ficheiro de leitura em data no do tipo:
// [ (valor_simb0;freq_simb0;bits_simb0;fp0) ; (valor_simb1;freq_simb1;bits_simb1;fp1) ; ...]
struct simbolo* converte1blocoData (FILE *fp, int bloco , int *tamBlocos){
    int i,c;

    struct simbolo *listaStruct; 
    listaStruct = (Nodo) malloc (sizeof(struct simbolo) * 256); //guarda espaço para todos os 256 simbolos


    fscanf(fp,"@%d@", &tamBlocos[bloco]);

    for (i=0 ; i<256; i++){
        
        c = fgetc(fp);

        if (c == ';') copiaFreqAnterior(listaStruct,i);
        else if (c == '@') {
            copiaFreqAnterior(listaStruct,i);
            fseek(fp,-1,SEEK_CUR);
        }
        else {
            fseek(fp,-1,SEEK_CUR);
            fscanf(fp,"%d",&listaStruct[i].freq);
            listaStruct[i].valor=i;
            (listaStruct[i]).bits = (int*)malloc(sizeof(int)*256);
            (listaStruct[i]).fp = -1;
            fseek(fp,1,SEEK_CUR);
        }
        
    }

return listaStruct;
}


//-----------------------------------------------------------------Ordenação dos dados para o tratamento-------------------------------------------------------------------------


// Troca elementos de uma lista
int swap (Nodo lista , int i , int m){

    struct simbolo tmp;
    tmp = lista[i];
    lista[i] = lista[m];
    lista[m] = tmp;

return 0;
}

// Ordena a lista de structs por frequencias descrescentemente
struct simbolo* ordenaStructPorFreq (Nodo lista){

    int i,j,m;

    for (i=0 ; i<255 ; i++){
        m = i;
        for (j = i+1 ; j<255 ; j++)
            if ( (lista[m].freq)<(lista[j].freq) ) m = j;
        if (i!=m) swap (lista,i,m);
    }

    return lista;
}

//-----------------------------------------------------------------Tabela shanon-Fano-------------------------------------------------------------------------


// Calcula a soma das frequencias de todos os símbolos 
int freqTotal (Nodo Lista , int start, int end){

    int soma=0,i;

    for (i=start ; i<end+1 ; i++)
        soma += Lista[i].freq;

    return soma;
}

// Calcula a soma das frequencias dos simbolos a partir de um certo indice até outro especifico
int somaFreqs (Nodo Lista , int start , int end){

    int soma = 0,i;

    for (i=start ; i<end+1 ; i++){
        soma += Lista[i].freq;
    }

return soma;
}

//Descobre em que indice se divide a lista de modo a dividir a lista equilibradamente
int indiceFreqMeio (Nodo Lista , int start , int end){

    int inicio, fim , i;
    int ind=0 , min=freqTotal(Lista,start,end);

    if (start-end == 1) return start; //Entre 2 elementos vai dividir sempre a meio

    for (i=start ; i<end ; i++){
        inicio = somaFreqs(Lista,start,i);
        fim = somaFreqs(Lista, i+1 ,end);

        if (abs(inicio-fim) < min){
            ind = i;
            min = abs(inicio-fim);
        }
    }

return ind;
}

// Adiciona o bit a sequencia de bits de um simbolo
void adiciona (int i , struct simbolo *lista , int indice){

    lista[indice].fp++;
    lista[indice].bits[lista[indice].fp] = i;

}

// Adiciona os bits de acordo com a Tabela de shanon-Fano
void shannonFano (Nodo lista , int start , int end){

    int i=0;
    int indFreqMeio = indiceFreqMeio(lista,start,end);

    if (end!=start){
        for (i=start ; i<indFreqMeio+1 ; i++)
            adiciona(0,lista,i);
        
        for (i; i<end+1 ; i++)
            adiciona(1,lista,i);

        shannonFano(lista,start,indFreqMeio);
        shannonFano(lista,indFreqMeio+1,end);

     }
}

//-----------------------------------------------------------------Ordenação dos dados para a escrita-------------------------------------------------------------------------



// Ordena a lista de structs pelo valor dos simbolos crescentemente
struct simbolo* ordenaStructPorSimbolos ( Nodo lista ){

    int i,j,m;

    for (i=0 ; i<255 ; i++){   //255
        m = i;
        for (j = i+1 ; j<256 ; j++) //256
            if ( (lista[m].valor)>(lista[j].valor) ) m = j;
        if (i!=m) swap (lista,i,m);
    }
    return lista;
}


//------------------------------------------------------------------------------Escrita--------------------------------------------------------------------------------------


// Escreve no ficheiro os 2 primeiros dados
void escreveInfoInicial (FILE *fp2 , char oriRle , int nrBlocos){

    fprintf(fp2,"@%c@%d",oriRle,nrBlocos);
}

// Escreve a sequencia de bits shanon-fano de um simbolo
void escreveSeqBits (FILE *fp2 , struct simbolo simbolo){

    int fp = simbolo.fp;
    int i = 0;

    while (fp>=0){
        fprintf(fp2,"%d",simbolo.bits[i]);

        i++;
        fp--;
    }
}

// Escreve no ficheiro todas as codificações shanon-fano de um bloco
void escreveBloco (FILE *fp2 , Nodo lista){

    int i;
    int j=0;
    
    for (i=0 ; i<255 ; i++){ //255
        escreveSeqBits(fp2, lista[i]);
        fprintf(fp2,";");
    }
    
    int fp = lista[i].fp;
    while (fp>=0){
        fprintf(fp2,"%d",lista[i].bits[j]);
        j++;
        fp--;
    }
}

// Escreve o tamanho do bloco
void escreveTamBloco (FILE *fp2 , int tamBloco){

    fprintf(fp2,"@%d@",tamBloco);

}

// Escreve as informações do trabalho na consola 
void textoDeConsola (char orig_rle , float time , int nrBlocos , int *tamBlocos , char *filename){

    int i,j=0;

    printf("Ana Catarina a93259, Mafalda Costa a83919 , Engenharia Informática , 3-Janeiro-2021\n");
    printf("Módulo: t (cálculo dos códigos dos símbolos)\n");
    printf("Número de blocos:%d\n",nrBlocos);

    printf("Tamanho dos blocos analisados no ficheiro de símbolo:");
    for (i=0 ; i<nrBlocos-1 ; i++)
        printf("%d/",tamBlocos[i]);
    printf("%d bytes\n",tamBlocos[i]);

    printf("Tempo de execução do módulo: %f milisegundos\n",time);

    printf("Ficheiro gerado: %s",filename);

    printf("\n");

}

// Serve para escolher o intervalo a partir da qual o shanon-fano vai trabalhar
int descobreOndeComecamFreqs0 (Nodo lista){
    int i=0;

    while(lista[i].freq!=0)
        i++;

    return i;
}



int moduloT (char *file){

    float clockStart = clock();
    float clockEnd;
    float time;
    int nrBlocos , i ,j;
    char origRle;
    int *freqBlocos;
    struct simbolo *listaStruct;
    int *tamBlocos;
    int end;
    char *filename;

    // Abre o ficheiro freq
    FILE *fp;
    fp = fopen( file , "r");

    for (j = 0; (file)[j]!='\0' ; j++ )

    filename = malloc(sizeof(char)*j);

    for (j = 0 ; (file)[j+4] != '\0' ; j++)
        filename[j] = (file)[j];

    strcat(filename,"cod");

    // Guarda numa variável se o ficheiro é RLE ou não
    origRle = ORIG_RLE(fp);

    // Guarda o número de blocos do ficheiro
    nrBlocos = descobre_nr_blocos(fp);
    tamBlocos = (int*)malloc(sizeof(int)*nrBlocos);

    // Abre o ficheiro onde vamos escrever
    FILE *fp2;
    fp2 = fopen (filename,"w");

    // Escreve os 2 primeiros dados (iguais ao ficheiro recebido)
    escreveInfoInicial(fp2,origRle,nrBlocos);

    // Lê, processa e escreve um bloco de cada vez
    for (i=0 ; i<nrBlocos; i++){
        listaStruct = converte1blocoData(fp,i,tamBlocos);
        listaStruct = ordenaStructPorFreq(listaStruct);
        end = descobreOndeComecamFreqs0(listaStruct);
        shannonFano(listaStruct,0,end-1);
        listaStruct = ordenaStructPorSimbolos(listaStruct);
        escreveTamBloco(fp2,tamBlocos[i]);
        escreveBloco (fp2,listaStruct);
    }

    // 2 últimos caracteres que simbolizam o fim do ficheiro
    fprintf(fp2,"@0");

    // Reporta o tempo que demorou
    clockEnd = clock();
    time = (double)(((clockEnd - clockStart)/CLOCKS_PER_SEC))*1000;

    // Informações na consola
    textoDeConsola(origRle,time,nrBlocos,tamBlocos,filename);

    // Fecha os ficheiros, tanto o de leitura como o de escrita
    fclose(fp);
    fclose(fp2);

    return 0;
}
