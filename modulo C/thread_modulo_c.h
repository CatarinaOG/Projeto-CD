
#ifndef MODULO_C_H
#define MODULO_C_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define CHECK(x) if(!(x)) {return -1;printf("ERRO!Verifique o ficheiro dado!");}
#define CBUFFER(x,buffer,fp) if (x == BREAD){fread(buffer,sizeof(unsigned char),BREAD,fp);c = 0;}  
#define BREAD (1024*2)

// Local onde é armazenado os codigos dos simbolos daquele bloco 
typedef struct{
	int tam, // Tamanho maximo do array dinâmico
	n; // Onde vais no used
	unsigned char used[256], // Local onde são guardados os codigos encontrados
	*arr; // Array dinâmico onde são armazenados todos os codigos
} darr,*pdarr;

//Numero de threads que o programa cria
#define NUM_THREADS 128
//Cria o array dinâmico
#define CREATE_DARR(x) CHECK(x = malloc(sizeof(darr)));x->tam = 1024;CHECK(x->arr = malloc(sizeof(unsigned char)*1024));
//Realoca a memoria no array dinâmico 
#define REALLOC_DARR(x,c,a) if(!(c+a < x->tam)){x->tam *= 2;CHECK(realloc(x->arr,sizeof(unsigned char)*x->tam));}
//Dá free a memoria do array dinâmico 
#define DEL_DARR(x) free(x->arr);free(x);

typedef struct {

	unsigned char *in, // Array por onde é lido o ficheiro original
	*out; // Array por onde é escrito a array final
	int *tblocos, // Valores pares deste array - tamanho do bloco ; Valores impares - tamanho maximo de um sibolo no bloco 
	bloco,//bloco que o thread esta a processar
	*cblocos; // Tamanho dos blocos depois de comprimido
	pdarr codes; // Local onde é armazenado os codigos dos simbolos daquele bloco 
} targ,*ptarg;  


//Função que dá print à informação relativa aos criadores e detalhes deste modulo
void print(char* name,int nblocos,float time,int *tblocos,int *cblocos);

//Função que lê o ficheiro dado a este modulo de forma a podermos interpretar a informaçõa presente no mesmo
int read(FILE *fp,int *tblocos,pdarr codes,unsigned char buffer[],int c);


//Função que cria uma matriz de codigos utilizando os offsets
int makeTable(unsigned char* table,pdarr codes,int tam);

//Função que utilizada todas as funções anteriores de forma a comprimir o ficheiro utilizando o metodo shannon-fano
int moduloC(char *path);
#endif