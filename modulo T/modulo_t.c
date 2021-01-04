#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

//-----------------------------------------------------------------LEITURA-------------------------------------------------------------------------

// Descobre se ocorreu RLE ou nao
char ORIG_RLE (FILE *fp){

    char orig_rle;
    fscanf(fp,"@%c@",&orig_rle);        //Guarda o 1º valor do ficheiro, R|N

return orig_rle;
}

// Recolhe o nr de blocos
int descobre_nr_blocos(FILE *fp){

    int nrBlocos;
    fscanf(fp,"%d",&nrBlocos);          //Guarda o 2º valor do ficheiro, o número de blocos

return nrBlocos;
}

typedef struct simbolo {
    int valor;                      // valor do simbolo
    int freq;                       // frequencia do simbolo
    int *bits;                      // sequencia de bits shanon-fano
    int fp;                         // guarda o indice do ultimo bit preenchido
}*Nodo;

// Preenche um campo da lista com frequência igual ao seu precendente
void copiaFreqAnterior (Nodo listaStruct , int i){

    listaStruct[i].valor = i;                                   //Parâmetro valor
    listaStruct[i].freq = listaStruct[i-1].freq;                //Parâmetro frequência igual ao do anterior
    (listaStruct[i]).bits = (int*)malloc(sizeof(int)*256);      //Guarda o espaço necessário para a sequência de bits
    (listaStruct[i]).fp = -1;                                   //Coloca o fp a -1 a indicar que a sequência de bits está vazia

}

// Converte 1 bloco do ficheiro de leitura em data do tipo:
// [ (valor_simb0;freq_simb0;bits_simb0;fp0) ; (valor_simb1;freq_simb1;bits_simb1;fp1) ; ...]
struct simbolo* converte1blocoData (FILE *fp, int bloco , int *tamBlocos){
    int i,c,n;

    struct simbolo *listaStruct; 
    listaStruct = (Nodo) malloc (sizeof(struct simbolo) * 256);    //Guarda espaço na lista para todos os 256 simbolos


    fscanf(fp,"@%d@", &n);                          // Guarda numa lista de tamanhos, o tamanho daquele bloco

    tamBlocos[bloco] = n;

    for (i=0 ; i<256; i++){
        
        c = fgetc(fp);                                             // Coloca na variável o char apontado por fp 
                                                                   // A este ponto é asegurado que nenhuma frequência é lida, visto que quando uma frequência é lida, ocorre o avanço do fp para a seguinte e não o ";" que lhe segue
        if (c == ';') copiaFreqAnterior(listaStruct,i);            // Sendo o caso de não se encontrar um float, copia a frequência do simbolo anterior
        else if (c == '@') {                                       
            copiaFreqAnterior(listaStruct,i);                      // Sendo o caso de aparecer um "@" copia a frequência anterior
            fseek(fp,-1,SEEK_CUR);                                 // Para normalizar a leitura do bloco seguinte voltamos a meter o fp a apontar para "@"
        }
        else {
            fseek(fp,-1,SEEK_CUR);                                      // Se o char não for nem ";" nem "@" então é um float e temos de apontar o fp para esse valor de novo
            fscanf(fp,"%d",&listaStruct[i].freq);                       // Preenche o campo de frequência com o valor apontado pelo fp
                                 // 
            listaStruct[i].valor=i;
            (listaStruct[i]).bits = (int*)malloc(sizeof(int)*256);
            (listaStruct[i]).fp = -1;
            c = fgetc(fp);
            if(c == '@') fseek(fp,-1,SEEK_CUR);                   // Avança o ";" depois de preenchido a estrutura do simbolo
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

// Ordena a lista de structs por frequencias descrescentemente (Função fornecida na cadeira de algoritmos de 2º ano da universidade do minho)
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


// Calcula a soma das frequencias dos simbolos a partir start até end, incluido
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
    int ind=0 , min=somaFreqs(Lista,start,end);         // Guarda um máximo que a soma das frequências pode ter

    if (start-end == 1) return start;                   //Entre 2 elementos vai sempre dividir a meio

    for (i=start ; i<end ; i++){                        // Vai realizar todas as possíveis divisões
        inicio = somaFreqs(Lista,start,i);              // Calcula para cada divisão a soma das frequências inciais
        fim = somaFreqs(Lista, i+1 ,end);               // Calcula para cada divisão a soma das frequências finais

        if (abs(inicio-fim) < min){                     // Verifica se se atingiu um mínimo
            ind = i;
            min = abs(inicio-fim);
        }
    }

return ind;
}

// Adiciona o bit a sequencia de bits de um simbolo
void adiciona (int i , struct simbolo *lista , int indice){

    lista[indice].fp++;                                     // É incrementado o fp a cada adição de bits
    lista[indice].bits[lista[indice].fp] = i;               // Coloca-se na posição apontada pelo fp o bit fornecido

}

// Adiciona os bits de acordo com a Tabela de shanon-Fano
void shannonFano (Nodo lista , int start , int end){

    int i=0;
    int indFreqMeio = indiceFreqMeio(lista,start,end);         // Guarda o índice onde se divide a lista 

    if (end!=start){
        for (i=start ; i<indFreqMeio+1 ; i++)
            adiciona(0,lista,i);                               // Adiciona 0's à primeira parte da lista
        
        for (; i<end+1 ; i++)
            adiciona(1,lista,i);                               // Adiciona 1's à segumda parte da lista

        shannonFano(lista,start,indFreqMeio);                  // Chamada recursiva para a primeira parte da lista
        shannonFano(lista,indFreqMeio+1,end);                  // Chamada recursiva para a segunda parte da lista

     }
}

//-----------------------------------------------------------------Ordenação dos dados para a escrita-------------------------------------------------------------------------



// Ordena a lista de structs pelo valor dos simbolos crescentemente (Função fornecida na cadeira de algoritmos de 2º ano da universidade do minho)
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
        
    for (i=0 ; i<255 ; i++){                    // Escreve todos os 255 simbolos + ";"
        escreveSeqBits(fp2, lista[i]);
        fprintf(fp2,";");
    }
    
    escreveSeqBits(fp2, lista[i]);              // Escreve a codificação do simbolo 255
}

// Escreve o tamanho do bloco
void escreveTamBloco (FILE *fp2 , int tamBloco){

    fprintf(fp2,"@%d@",tamBloco);

}

// Escreve as informações do trabalho na consola 
void textoDeConsola (char orig_rle , float time , int nrBlocos , int *tamBlocos , char *filename){

    int i;

    printf("Ana Catarina a93259 , MIEI/CD, 3-Janeiro-2021\nMafalda Costa a83919 , MIEI/CD, 3-Janeiro-2021\n");
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

// Encontra o último elemento que têm frequência maior que zero de modo a não fazer a codificação desses simbolos com frequência igual a 0
int descobreOndeComecamFreqs0 (Nodo lista){
    int i=0;

    while(lista[i].freq!=0 && i<256)
        i++;

    return i;
}



int moduloT (char *file){

    float clockStart = clock();
    float clockEnd;
    float time;
    int nrBlocos , i ,j;
    char origRle;
    struct simbolo *listaStruct = NULL;
    int *tamBlocos = NULL;
    int end;
    char *filename = NULL;

   
    FILE *fp;                                   // Abre o ficheiro freq
    fp = fopen( file , "r");          

    for (j = 0; (file)[j]!='\0' ; j++ )      // Descobre o tamanho do nome do ficheiro

    filename = malloc(sizeof(char)*j);          // Guarda o espaço necesário para o nome do ficheiro resultante 

    for (j = 0 ; (file)[j+4] != '\0' ; j++)  // Copia o nome do ficheiro sem o "freq" para a lista filename
        filename[j] = (file)[j];
    strcat(filename,"cod");                     // Adiciona o "cod" no nome do ficheiro original

    origRle = ORIG_RLE(fp);                     // Guarda numa variável se o ficheiro é RLE ou não
    nrBlocos = descobre_nr_blocos(fp);          // Guarda o número de blocos do ficheiro

    tamBlocos = (int*)malloc(sizeof(int)*nrBlocos);     // Guarda espaço para uma lista para preencher com o tamanho dos blocos

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

    // Escreve os 2 últimos caracteres que simbolizam o fim do ficheiro
    fprintf(fp2,"@0");

    // Reporta o tempo que demorou
    clockEnd = clock();
    time = (double)(((clockEnd - clockStart)/CLOCKS_PER_SEC))*1000;

    // Informações na consola
    textoDeConsola(origRle,time,nrBlocos,tamBlocos,filename);

    // Fecha os ficheiros, tanto o de leitura como o de escrita
    fclose(fp);
    fclose(fp2);
    
    for (int k=0 ; k<256 ; k++)
    free(listaStruct[k].bits);
    
    free(listaStruct);
    free(tamBlocos);

    return 0;
}
