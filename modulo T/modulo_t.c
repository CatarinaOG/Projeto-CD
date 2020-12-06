#include <stdio.h>
#include <stdlib.h>
#define MAX 100

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

// converte 1 bloco do ficheiro de leitura em data no código
int converte1bloco (FILE *fp , int nrBlocos , int orig_rle){
    int *tam_blocos;
    int *freq_blocos;
    int i,c;

    freq_blocos = malloc (sizeof(int)*256 + 1);

    fscanf(fp,"@%d@", &freq_blocos[0]);

    for (i=1 ; i<256; i++){

        c = fgetc(fp);

        if ( c == ';' ) freq_blocos[i] = freq_blocos[i-1];
        else {
            fseek(fp,-1,SEEK_CUR);
            fscanf(fp,"%d;",&freq_blocos[i]);
        }
    }


// testes
    printf("%d\n",freq_blocos[0]);
    printf("%d\n",freq_blocos[1]);
    printf("%d\n",freq_blocos[2]);
    printf("%d\n",freq_blocos[3]);
    printf("%d\n",freq_blocos[4]);
    printf("%d\n",freq_blocos[5]);
    printf("%d\n",freq_blocos[6]);
    printf("%d\n",freq_blocos[100]);
    printf("%d\n",freq_blocos[101]);


return 0;

}


int main (){

    int nrBlocos , i;
    char orig_rle;

    FILE *fp;
    fp = fopen( "aaa.txt.freq" , "r");

    orig_rle = ORIG_RLE(fp); // Original ou RLE?
    nrBlocos = descobre_nr_blocos(fp); // Nr de blocos

    printf("rle: %c    nrblocos: %d \n",orig_rle, nrBlocos);

    for (i=0 ; i<nrBlocos; i++){
        printf("Bloco%d:",i);
        converte1bloco(fp,nrBlocos,orig_rle);
    }
    return 0;

}