
#ifndef MODULO_C_H
#define MODULO_C_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHECK(x) if(!(x)) return -1;
#define BREAD (1024)

void ptable(unsigned char *table,int tam);

void pread(int tblocos[],unsigned char* codes[],int nblocos);

void print(char* name,int nblocos,double time,int *tblocos,int *cblocos);

int read1(char* path,int* tblocos[],unsigned char** codes[]);

int read(char* path,int* tblocos[],unsigned char** codes[]);

int makeTable(unsigned char* table,unsigned char* codes[],int tam);

int encode(char *path,char  *pathcod);

#endif