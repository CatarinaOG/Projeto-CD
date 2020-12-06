#include <stdio.h>
#include <stdlib.h>

// Descobre o número blocos


// Descobre se ocorreu RLE ou nao
char ORIG_RLE (FILE *fp){

    char orig_rle;
    fscanf(fp,"@%c@",&orig_rle);

return orig_rle;
}

// recolhe o nr de blocos
int descobre_nr_blocos(FILE *fp){

    int nrBlocos;
    fscanf(fp,"%d",&nrBlocos);

return nrBlocos;
}

// converte 1 bloco do ficheiro de leitura em data no código do tipo:
// [tambloco ; freq_simb0 ; freq_simb1 ; freq_simb2 ; ...]
int *converte1blocoData (FILE *fp , int nrBlocos , int orig_rle){
    int *tam_blocos;
    int *freq_blocos;
    int i,c;

    freq_blocos = malloc (sizeof(int)*256 + 1);

    fscanf(fp,"@%d@", &freq_blocos[0]);
    fscanf(fp,"%d;",&freq_blocos[1]);

    for (i=2 ; i<5; i++){  //257
        
        c = fgetc(fp);

        if (c == ';') freq_blocos[i] = freq_blocos[i-1];
        else if (c == '@') {
            freq_blocos[i] = freq_blocos[i-1];
            fseek(fp,-1,SEEK_CUR);
        }
        else {
            fseek(fp,-1,SEEK_CUR);
            fscanf(fp,"%d",&freq_blocos[i]);
            fseek(fp,1,SEEK_CUR);
        }
        
    }

fseek(fp,-1,SEEK_CUR);

// testes
    // printf("%d\n",freq_blocos[0]);
    // printf("%d\n",freq_blocos[1]);
    // printf("%d\n",freq_blocos[2]);
    // printf("%d\n",freq_blocos[3]);
    // printf("%d\n",freq_blocos[4]);
    //printf("%d\n",freq_blocos[5]);
    // printf("%d\n",freq_blocos[254]);
    // printf("%d\n",freq_blocos[255]);
    // printf("%d\n",freq_blocos[256]);
    //printf("%d\n",freq_blocos[100]);
    //printf("%d\n",freq_blocos[101]);

return freq_blocos;
}


typedef struct simbolo {
    int valor;
    int freq;
    long bits;
}*Nodo;

// converte uma lista de frequencias em lista de structs:
// [ (valor_simb0;freq_simb0;bits_simb0) ; (valor_simb1;freq_simb1;bits_simb1) ; ...]

struct simbolo *converteEmStruct (int *freqBlocos){
    int i;
    struct simbolo *listaStruct; 
    listaStruct = malloc (sizeof(struct simbolo) * 256);

    for (i=0 ; i<5 ; i++){  //256
        (listaStruct[i]).valor = i;
        (listaStruct[i]).freq = freqBlocos[i+1];
    }

    return listaStruct;
} 

// Troca elementos de uma lista
int swap (Nodo lista , int i , int m){

    struct simbolo tmp;
    tmp = lista[i];
    lista[i] = lista[m];
    lista[m] = tmp;

return 0;
}

//ordena a lista de structs por frequencias descrescentemente
struct simbolo* ordenaStructFreq (Nodo lista){

    int i,j,m;

    for (i=0 ; i<4 ; i++){   //255
        m = i;
        for (j = i+1 ; j<255 ; j++)
            if ( (lista[m].freq)<(lista[j].freq) ) m = j;
        if (i!=m) swap (lista,i,m);
    }

//testes
    // printf("valor: %d\n",lista[0].valor);
    // printf("freq: %d\n",lista[0].freq);
    // printf("valor: %d\n",lista[1].valor);
    // printf("freq: %d\n",lista[1].freq);
    // printf("valor: %d\n",lista[2].valor);
    // printf("freq: %d\n",lista[2].freq);
    // printf("valor: %d\n",lista[3].valor);
    // printf("freq: %d\n",lista[3].freq);

    return lista;
}

//calcula a soma das frequencias de todos os símbolos
int freqTotal (Nodo Lista){

    int soma=0,i;

    for (i=0 ; i<256 ; i++)
        soma += Lista[i].freq;

    return soma;
}

//Calcula a soma das frequencias dos simbolos a partir de um certo indice até outro especifico
int somaFreqs (Nodo Lista , int start , int end){

    int soma = 0,i;

    for (i=start ; i<end ; i++) 
        soma += Lista[i].freq;

return soma;
}

// Descobre em que indice se divide a lista de modo a dividir as frequencias mais equilibradamente
int indiceFreqMeio (Nodo Lista){

    int conta = 0,i=0;
    int inicio, fim;
    int ind=0 , min=freqTotal(Lista);

    for (i=0 ; i<256 ; i++){
        inicio = somaFreqs(Lista,0,i+1);
        fim = somaFreqs(Lista, i+1 ,256);

        if (abs(inicio-fim) < min){
            ind = i;
            min = abs(inicio-fim);
        }
    }

return ind;
}


// int shannonFano (Nodo lista , int tamBloco){

//     while (encontra_ind_metade)

// }


int main (){

    int nrBlocos , i;
    char orig_rle;
    int *freqBlocos;
    int tabela;
    struct simbolo *listaStruct;
    int c;

    FILE *fp;
    fp = fopen( "aaa.txt.freq" , "r");

    orig_rle = ORIG_RLE(fp); // Original ou RLE?
    nrBlocos = descobre_nr_blocos(fp); // Nr de blocos

    //printf("rle: %c    nrblocos: %d \n",orig_rle, nrBlocos);

    for (i=0 ; i<nrBlocos; i++){
        printf("\nBloco%d:\n",i);
        freqBlocos = converte1blocoData(fp,nrBlocos,orig_rle);
        listaStruct = converteEmStruct(freqBlocos);
        listaStruct = ordenaStructFreq(listaStruct);
        printf("freqT:%d ",freqTotal(listaStruct));
        c= indiceFreqMeio(listaStruct);
    }
    return 0;
}
