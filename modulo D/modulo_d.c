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
void print(char *path, char valor_rle, int n_rep){
    FILE *fl = fopen(path,"w");    //lê desde o inicio, acrescenta só no fim
    for(int i = 0;i < n_rep; i++)
        fprintf(fl,"%c",valor_rle); //(ASCII->Valor) 
    fclose(fl);
}

//
void decompressRLE(char *path){
    FILE *fp_rle;
    char ch, str[2], path_new[PATH_MAX_SIZE];
    fp_rle = fopen(path,"r");
    removeCharsFromPath(path,path_new,4);
    while((ch = fgetc(fp_rle)) != EOF){
        if(ch != '\0'){
            fgets(str,2,fp_rle);
            print(path_new,str[1],(int) str[2]);
        }
        else print(path_new,ch,1);
    }
    fclose(fp_rle);
}

int main(){
    char path[] = "aaa.txt.rle";
    decompressRLE(path);
    return 0;
}