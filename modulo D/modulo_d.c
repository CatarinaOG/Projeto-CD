#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PATH_MAX_SIZE 1024

//Verifica se houve compressão RLE
char checkRLE(FILE *fp){
    fseek(fp,1,SEEK_SET);
    return (char) fgetc(fp);
}

//Descobre nº de blocos em que o ficheiro foi dividido
int nrBlocos(FILE *fp){
    int nr_blocos;
    fseek(fp,3,SEEK_SET);
    fscanf(fp,"%d",&nr_blocos);
    return nr_blocos;
}

//Remove n chars do path
//Exemplo: Se tivermos como inputs path = "aaa.txt.rle" e n = 4, temos como resultado path = "aaa.txt"
//Return 1 caso seja bem sucedido, se o return for 0 enta mt provavelmente o path não está corretamente formatado
void removeCharsFromPath (char *path, char *path_new, int n){
    int i;
    for(i = 0;path[i] != '\0';i++);
    strncpy(path_new,path,i-n);
}


/************** RLE ***************/

//função que imprime valores de rle repetidos
void print(FILE *fp, char valor_rle, int n_rep){
    for(int i = 0;i < n_rep; i++)
        fputc(valor_rle,fp); //(ASCII->Valor)
}

//Faz descompressão RLE (Cria um novo ficheiro)
void decompressRLE(char *path){
    FILE *fp_rle, *fp_new;
    char ch, path_new[PATH_MAX_SIZE];
    int nr_rep;
    removeCharsFromPath(path,path_new,4);
    fp_rle = fopen(path,"r");
    fp_new = fopen(path_new,"a+");    //lê desde o inicio, acrescenta só no fim
    while((ch = (char)fgetc(fp_rle)) != EOF){
        if(ch == '\0'){
            ch     = (char)fgetc(fp_rle);
            nr_rep = fgetc(fp_rle);
            print(fp_new,ch,nr_rep);
        }
        else fputc(ch,fp_new);
    }
    fclose(fp_rle);
    fclose(fp_new);
}


/************** SHANNON-FANO ***************/

//'C' representa que passará a ler as sequencias 
//    de bits que representam os diversos chars
//'T' significa que irá ler o tamanho do bloco
void mudaInputType(char *inputType){
    if((*inputType) == 'C') (*inputType) = 'T';
    else (*inputType) = 'C';
}

//Retorna o fp com a posicao do tamanho do bloco pretendido
//Se nr_bloco chegar ao valor 0 então encontra-se na posicao correta e dá return ao fp
void pointerBloco(FILE *fp, int nr_bloco){
    char ch;
    int a;
    fseek(fp,3,SEEK_SET); //Dá skip a @<N|R>@
    fscanf(fp,"%d",&a);   //Skip ao nº de blocos
    fseek(fp,1,SEEK_CUR); //Skip ao @
    if(nr_bloco == 0) return;
    char inputType = 'T';
    while(1){
        if((ch = (char) fgetc(fp)) == '@'){
            mudaInputType(&inputType);
            if(inputType == 'T'){
                nr_bloco--;
                if(nr_bloco==0) return;
            }
        }
    }
}

//Analiza o bloco, obtendo os comprimentos das diferentes sequencias de bits(array "tam"), os chars correspondentes a cada uma (array "chars") 
//e as próprias sequencias (lista de strings "codes")
//fp tem de apontar para o inicio do bloco
void analizaBloco(FILE *fp, int *tam, char *chars, char **codes, int *nr_codes){
    //tam    = (int *)   malloc(sizeof(int)*256);    //É suposto inicializar na funcao onde é criada(alterar para 16 caso consiga pôr o realloc a funcionar)
    //chars  = (char *)  malloc(sizeof(char)*256);   //É suposto inicializar na funcao onde é criada
    //codes  = (char **) malloc(sizeof(char *)*256); //É suposto inicializar na funcao onde é criada
    //(*nr_codes) = 0; //É suposto inicializar na funcao onde é criada 
    char a[256],
         ch;
    int  checkSpace,
         tam_aux = 0,  
         _ch = 0,
         i = 0; //indice que indica o char
    while((ch = (char) fgetc(fp)) != '@'){
        if(ch == ';'){
            if(tam_aux != 0){
                tam[_ch]   = tam_aux;
                codes[_ch] = (char *) malloc(sizeof(char)*(tam_aux+1));
                chars[_ch] = (char) i;
                a[tam_aux] = '\0';
                strcpy(codes[_ch],a);
                tam_aux = 0;
                _ch++;
            }
            i++;
        }
        else {
            a[tam_aux] = ch;
            tam_aux++;
        }
    }
    if(tam_aux != 0){
        codes[_ch] = (char *) malloc(sizeof(char)*(tam_aux+1));
        chars[_ch] = (char) i;
        tam[_ch]   = tam_aux;
        a[tam_aux] = '\0';
        strcpy(codes[_ch],a);
        _ch++;
    }
    (*nr_codes) = _ch;
    //for(int j = 0; j<=_ch; j++){
    //    printf("j = %03d /tam = %02d / str = %s / char = %c\n",j,tam[j],codes[j],chars[j]);
    //}
}

//Faz descompressao SF (cria ficheiro do tipo .rle ou original)
void decompressSF(char *path_cod, char *path_shaf){
    //FILE *fp_shaf = fopen(path_shaf,"r"), 
    //     *fp_cod  = fopen(path_cod,"r"),
    //     *fp_new;
    //char check_RLE = checkRLE(fp_cod);
    //int  nr_blocos = nrBlocos(fp_cod),
    //     tam;
    return;
}

int main() {
    clock_t tic = clock();
    FILE *fp = fopen("aaa.txt.rle.cod","a+");
    pointerBloco(fp,1);
    int a;
    fscanf(fp,"%d",&a);
    fseek(fp,1,SEEK_CUR);
    int *tam; char **codes, *chars;
    int nr_codes = 0;
    analizaBloco(fp,tam,chars,codes,&nr_codes);
    fclose(fp);
    clock_t toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    return 0;
}