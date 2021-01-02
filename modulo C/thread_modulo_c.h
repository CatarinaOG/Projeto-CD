
#ifndef MODULO_C_H
#define MODULO_C_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <pthread.h>

#define CHECK(x) if(!(x)) {return -1;printf("");}
#define CBUFFER(x,buffer,fp) if (x == BREAD){fread(buffer,sizeof(unsigned char),BREAD,fp);c = 0;}  
#define BREAD (1024*2)

typedef struct{
	int tam,n;
	unsigned char used[256],*arr;
} darr,*pdarr;

#define CREATE_DARR(x) CHECK(x = malloc(sizeof(darr)));x->tam = 1024;CHECK(x->arr = malloc(sizeof(unsigned char)*1024));
#define REALLOC_DARR(x,c,a) if(!(c+a < x->tam)){x->tam *= 2;CHECK(realloc(x->arr,sizeof(unsigned char)*x->tam));printf("Realloc\n");}
#define DEL_DARR(x) free(x->arr);free(x);

typedef struct {
	unsigned char *in,*out;
	int *tblocos,*cblocos,bloco;
	pdarr codes;
} targ,*ptarg;  

void print(char* name,int nblocos,float time,int *tblocos,int *cblocos);

int read(FILE *fp,int *tblocos,pdarr codes,unsigned char buffer[],int c);

int makeTable(unsigned char* table,pdarr codes,int tam);

int moduloC(char *path);

#endif