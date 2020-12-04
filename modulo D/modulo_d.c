#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define PATH_MAX_SIZE 1024

//Remove n chars do path
//Exemplo: Se tivermos como inputs path = "aaa.txt.rle" e n = 4, temos como resultado path = "aaa.txt" 
//Return 1 caso seja bem sucedido, se o return for 0 enta mt provavelmente o path não está corretamente formatado
void removeCharsFromPath (char *path, char *path_new, int n){
    int i;
    for(i = 0;path[i] != '\0';i++);
    strncpy(path_new,path,i-n);
}

//função que imprime valores de rle repetidos
void print(FILE *fp, char valor_rle, int n_rep){
    for(int i = 0;i < n_rep; i++)
        fprintf(fp,"%c",valor_rle); //(ASCII->Valor) 
}

//
void decompressRLE(char *path){
    FILE *fp_rle, *fp_new;
    char ch, str[2], path_new[PATH_MAX_SIZE];
    removeCharsFromPath(path,path_new,4);
    fp_rle = fopen(path,"r");
    fp_new = fopen(path_new,"a+");    //lê desde o inicio, acrescenta só no fim
    while((ch = fgetc(fp_rle)) != EOF){
        if(ch == '\0'){
            fgets(str,2,fp_rle);
            print(fp_new,str[1],(int)str[2]);
        }
        else fputc(ch,fp_new);
    }
    fclose(fp_rle);
    fclose(fp_new);
}

int main(){
    char path[] = "aaa.txt.rle";
    decompressRLE(path);
    return 0;
}