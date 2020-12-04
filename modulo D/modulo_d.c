#include <stdio.h>
#include <stdlib.h>

//Remove n chars do path
//Exemplo: Se tivermos como inputs path = "aaa.txt.rle" e n = 4, temos como resultado path = "aaa.txt" 
//Return 1 caso seja bem sucedido, se o return for 0 enta mt provavelmente o path não está corretamente formatado
int removeCharsFromPath (char *path, int n){
    int i, r = 1, aux = i - n;
    for(i = 0;path[i] != '\0';i++);
    if(aux < 0) r = 0;
    else path[i-n] = '\0';
    return r;
}

//
void decompressRLE(char *path){
    FILE *fp_rle;
    char ch, str[2], *path_new;
    fp_rle = fopen(path,"r");
    removeCharsFromPath(path_new,4);
    while((ch = fgetc(fp_rle)) != EOF){
        if(ch != '\0'){
            fgets(str,2,fp_rle);
            print(path_new,str[1],(int) str[2]);
        }
        else print(path_new,ch,1);
    }
}