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

//Descobre nº de blocos em que o ficheiro original/rle foi dividido
//E deixa o fp na posicao ler o tamanho do 1º bloco
int nrBlocosCod(FILE *fp){
    int nr_blocos;
    fseek(fp,1,SEEK_CUR); //Skip ao @ que segue <N|R>
    fscanf(fp,"%d",&nr_blocos);
    fseek(fp,1,SEEK_CUR); //Skip ao @
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

void inicializa_arr(int **tam, char **chars, char ***codes, int N){
    *tam    = (int *)   malloc(sizeof(int)*N);    //É suposto inicializar na funcao onde é criada(alterar para 16 caso consiga pôr o realloc a funcionar)
    *chars  = (char *)  malloc(sizeof(char)*N);
    *codes  = (char **) malloc(sizeof(char *)*N);
}

//Retorna o fp com a posicao do tamanho do bloco seguinte
//
void pointerBlocoCod(FILE *fp, int nr_bloco){
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

void skipNrBlocosShaf(FILE *fp){
    fseek(fp,1,SEEK_SET); //Skip ao @
    int a;
    fscanf(fp,"%d",&a);   //Skip ao nº de blocos
    fseek(fp,1,SEEK_CUR); //Skip ao @
    return;
}

void tamanhoBloco(FILE *fp, int *tam_bloco){
    fscanf(fp,"%d",tam_bloco);
    fseek(fp,1,SEEK_CUR); //Dá skip ao @, para que fique pronto a ler o bloco
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

void swap_int(int* a, int* b){
    int t = *a;
    *a = *b;
    *b = t;
}

void swap_char(char* a, char* b){
    char t = *a;
    *a = *b;
    *b = t;
}

void swap_char_pointers(char** a, char** b){
    char* t = *a;
    *a = *b;
    *b = t;
}

// funçao auxiliar ao quicksort
int partition (int *tam, char *chars, char **codes, int low, int high)
{
    int pivot = tam[high];
    int i = (low - 1);
    for (int j = low; j <= high- 1; j++)
    {
        if (tam[j] < pivot)
        {
            i++;
            swap_int(&tam[i], &tam[j]);
            swap_char(&chars[i],&chars[j]);
            swap_char_pointers(&codes[i], &codes[j]);
        }
    }
    swap_int(&tam[i + 1], &tam[high]);
    swap_char(&chars[i + 1], &chars[high]);
    swap_char_pointers(&codes[i + 1], &codes[high]);
    return (i + 1);
}

// algoritmo quicksort obtido do site https://www.geeksforgeeks.org/quick-sort/, alterado para ordenar tanto uma lista de ints como de chars
void quickSort(int *tam, char *chars, char **codes, int low, int high){
    if (low < high){
        int pi = partition(tam, chars, codes, low, high);
        quickSort(tam, chars, codes, low, pi - 1);
        quickSort(tam, chars, codes, pi + 1, high);
    }
}


void auxDecompressSF(unsigned char *buffer_shaf, char *buffer_new, char *chars, char **codes, int nr_codes, int *tam_buffer_new){
    char str[256];
    int i  , _bn = 0; //_bn = indice buffer_new    _bs = indice buffer_shaf
    int shifts=0, _bs=0, prox_index=0;
    unsigned char aux= buffer_shaf[0];
    while(_bn < *tam_buffer_new){
        if(shifts==7){
            shifts=0;
            _bs++;
            aux= buffer_shaf[_bs];
        }

        addBit(str,&aux,_bs,&shifts,&prox_index);
        for(i=0; i<nr_codes && strcmp(str,codes[i]) ; i++);
        if(i<nr_codes){
            buffer_new[_bn] = chars[i];
            _bn++;
            prox_index =0;
        }
    }
}

//adiciona um bit 
void addBit( char *str,unsigned char *aux,int *shifts, int *prox_index){
    if(*aux< 128) str[prox_index] = '0';
    else str[prox_index] ='1';
    str[prox_index+1] ='\0';
    *prox_index++;
    *aux << 1;
    *shifts++;
}

//Faz descompressao SF (cria ficheiro do tipo .rle ou original)
void decompressSF(char *path_cod, char *path_shaf){
    char path_new[PATH_MAX_SIZE];
    removeCharsFromPath(path_shaf,path_new,4);
    FILE *fp_shaf = fopen(path_shaf,"rb"), 
         *fp_cod  = fopen(path_cod,"r"),
         *fp_new  = fopen(path_new,"w");
    unsigned char *buffer_shaf;
    char **codes, *chars, *buffer_new, check_RLE = checkRLE(fp_cod);
    int  *tam, i = 0, nr_codes = 0, nr_blocos = nrBlocosCod(fp_cod), tam_bloco_new, tam_bloco_shaf;
    skipNrBlocosShaf(fp_shaf);
    inicializa_arr(&tam, &chars, &codes,256);
    while(i < nr_blocos){
        tamanhoBloco(fp_cod,&tam_bloco_new);
        buffer_new = (char *) malloc(sizeof(char)*tam_bloco_new);
        tamanhoBloco(fp_shaf,&tam_bloco_shaf);
        buffer_shaf = (unsigned char *) malloc(sizeof(unsigned char)*tam_bloco_shaf);
        fread(buffer_shaf, sizeof(unsigned char), tam_bloco_shaf, fp_shaf); fseek(fp_shaf,1,SEEK_CUR); //Dá skip ao '@' para ficar a apontar para o tamanho do prox bloco(n deve haver problema com o EOF)
        analizaBloco(fp_cod,tam,chars,codes,&nr_codes); //O fp tem que ficar a apontar para o tamanho do proximo bloco(cuidado quando alterar para ler com buffer)
        quickSort(tam,chars,codes,0,nr_codes-1);
        auxDecompressSF(buffer_shaf,buffer_new,chars,codes,nr_codes);
        fwrite(buffer_new,sizeof(char),tam_bloco_new,fp_new);
        free(buffer_new);
        free(buffer_shaf);
        nr_codes = 0;
    }
    fclose(fp_shaf); 
    fclose(fp_cod); 
    fclose(fp_new); 
}

int main() {
    clock_t tic = clock();
    //FILE *fp = fopen("aaa.txt.rle.cod","r");
    //for(int j = 0; j<nr_codes; j++){
    //       printf("j = %03d /tam = %02d / str = %s / char = %c\n",j,tam[j],codes[j],chars[j]);
    //    }
    //    fclose(fp);
    decompressSF("aaa.txt.rle.cod","aaa.txt.rle.shaf");
    clock_t toc = clock();
    printf("Elapsed: %f seconds\n", (double)(toc - tic) / CLOCKS_PER_SEC);
    return 0;
}
