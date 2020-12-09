#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


char checkRLE(FILE *fp){
    fseek(fp,1,SEEK_SET);
    return (char) fgetc(fp);
}

int nrBlocosCod(FILE *fp){
    int nr_blocos;
    fseek(fp,1,SEEK_CUR); /*Skip ao @ que segue <N|R>*/
    fscanf(fp,"%d",&nr_blocos);
    fseek(fp,1,SEEK_CUR); /*Skip ao @*/
    return nr_blocos;
}

void removeExtensao (char *path, char **path_new, int n){
    int i;
    for(i = 0;path[i] != '\0';i++);
    i = i - n + 1; /* tamanho da string new */
    *path_new = (char *) malloc(i);
    strncpy(*path_new,path,i-1);
}

void substituiExtensao(char *path, char **path_new, char *ext_new, int n){
    removeExtensao(path,path_new,n);
    strcat(*path_new,ext_new);
}

void mudaInputType(char *inputType){
    if((*inputType) == 'C') (*inputType) = 'T';
    else (*inputType) = 'C';
}

/************** RLE ***************/

void print(FILE *fp, char valor_rle, int n_rep){
    int i;
    for(i = 0;i < n_rep; i++)
        fputc(valor_rle,fp); /*(ASCII->Valor)*/
}

int *bufferSizesRLE(FILE *fp, int *nr_blocos){
    char ch, inputType = 'C';
    int _bloco = 0, skip = 1; /*A variável skip é booleana, permite saber se podemos dar um skip de 256 chars(valor mínimo de cada bloco do ficheiro .freq)*/
    
    fseek(fp,3,SEEK_SET); /* Skip @<R|N>@ */
    fscanf(fp,"%d",nr_blocos); 

    int *buffer_sizes_rle = (int *) malloc((*nr_blocos)*sizeof(int));

    while(1){
        if((ch = (char) fgetc(fp)) == '@'){
            mudaInputType(&inputType);
            skip = 1;
            if(inputType == 'T'){
                if((char) fgetc(fp) == '0') return buffer_sizes_rle;
                fseek(fp,-1,SEEK_CUR);
                fscanf(fp,"%d",&(buffer_sizes_rle[_bloco]));
                _bloco++;
            }
            else {
                if(skip == 1) {
                    fseek(fp,256,SEEK_CUR);
                    skip = 0;    
                }
            }
        }
    }
    return buffer_sizes_rle;
}

void decompressRLE(FILE *fp_rle, FILE *fp_new, int *buffer_sizes_rle, int *nr_blocos){
    char ch;
    int _bloco, _buffer, nr_rep, tam_bloco;

    for(_bloco = 0; _bloco < (*nr_blocos); _bloco++){
        tam_bloco = buffer_sizes_rle[_bloco];
        char *buffer_rle = (char *) malloc(sizeof(char)*tam_bloco);
        fread(buffer_rle, sizeof(char), tam_bloco, fp_rle);

        for(_buffer = 0; _buffer < tam_bloco ; _buffer++){
            if(buffer_rle[_buffer] == '\0'){
                ch     = buffer_rle[++_buffer];
                nr_rep = (int) buffer_rle[++_buffer];
                print(fp_new,ch,nr_rep);
            }
            else fputc(buffer_rle[_buffer],fp_new);
        }

        free(buffer_rle);
    }
}


/************** SHANNON-FANO ***************/

void inicializa_arr(int **tam, char **chars, char ***codes, int N){
    *tam    = (int *)   malloc(sizeof(int)*N);    /*É suposto inicializar na funcao onde é criada(alterar para 16 caso consiga pôr o realloc a funcionar)*/
    *chars  = (char *)  malloc(sizeof(char)*N);
    *codes  = (char **) malloc(sizeof(char *)*N);
}

void skipNrBlocosShaf(FILE *fp){
    fseek(fp,1,SEEK_SET); /*Skip ao @*/
    int a;
    fscanf(fp,"%d",&a);   /*Skip ao nº de blocos*/
    fseek(fp,1,SEEK_CUR); /*Skip ao @*/
    return;
}

void tamanhoBloco(FILE *fp, int *tam_bloco){
    fscanf(fp,"%d",tam_bloco);
    fseek(fp,1,SEEK_CUR); /*Dá skip ao @, para que fique pronto a ler o bloco*/
}

void analizaBloco(FILE *fp, int *tam, char *chars, char **codes, int *nr_codes){
    /*tam    = (int *)   malloc(sizeof(int)*256);    //É suposto inicializar na funcao onde é criada(alterar para 16 caso consiga pôr o realloc a funcionar)
      chars  = (char *)  malloc(sizeof(char)*256);   //É suposto inicializar na funcao onde é criada
      codes  = (char **) malloc(sizeof(char *)*256); //É suposto inicializar na funcao onde é criada
      (*nr_codes) = 0; //É suposto inicializar na funcao onde é criada */
    char a[256],
         ch;
    int  tam_aux = 0,  
         _ch = 0,
         i = 0; /*indice que indica o char*/
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
    /*for(int j = 0; j<=_ch; j++){
          printf("j = %03d /tam = %02d / str = %s / char = %c\n",j,tam[j],codes[j],chars[j]);
      }*/
}

void swap(void *a, void *b, size_t s){
    void *tmp = malloc(s);
    memcpy(tmp,a,s);
    memcpy(a,b,s);
    memcpy(b,tmp,s);
    free(tmp);
}

int partition (int *tam, char *chars, char **codes, int low, int high){
    int pivot = tam[high];
    int i = (low - 1);
    int j;
    for (j = low; j <= high- 1; j++){
        if (tam[j] < pivot){
            i++;
            swap(&tam[i], &tam[j], sizeof(int));
            swap(&chars[i],&chars[j], sizeof(char));
            swap(&codes[i], &codes[j], sizeof(char*));
        }
    }
    swap(&tam[i + 1], &tam[high], sizeof(int));
    swap(&chars[i + 1], &chars[high], sizeof(char));
    swap(&codes[i + 1], &codes[high], sizeof(char*));
    return (i + 1);
}

void quickSort(int *tam, char *chars, char **codes, int low, int high){
    if (low < high){
        int pi = partition(tam, chars, codes, low, high);
        quickSort(tam, chars, codes, low, pi - 1);
        quickSort(tam, chars, codes, pi + 1, high);
    }
}

void addBit(char *str,unsigned char *aux,int *shifts, int *prox_index){
    if((*aux) < 128) str[*prox_index] = '0';
    else str[*prox_index] = '1';
    str[(*prox_index)+1] = '\0';
    (*prox_index)++;
    (*aux) = ((*aux) << 1);
    (*shifts)++;
}

void auxDecompressSF(unsigned char *buffer_shaf, char *buffer_new, char *chars, char **codes, int nr_codes, int *tam_buffer_new){
    char str[256];
    int i  , _bn = 0; /*_bn = indice buffer_new    _bs = indice buffer_shaf*/
    int shifts=0, _bs=0, prox_index=0;
    unsigned char aux = buffer_shaf[0];
    while(_bn < *tam_buffer_new){
        if(shifts > 7){
            shifts = 0;
            _bs++;
            aux = buffer_shaf[_bs];
        }
        addBit(str,&aux,&shifts,&prox_index);
        for(i=0; i<nr_codes && strcmp(str,codes[i]) ; i++);
        if(i<nr_codes){
            buffer_new[_bn] = chars[i];
            _bn++;
            prox_index = 0;
        }
    }
}

/*Faz descompressao SF (cria ficheiro do tipo .rle ou original)
  tam_blocos_new == NULL implica que não há descompressão RLE*/
void decompressSF(FILE *fp_shaf, FILE *fp_cod, FILE *fp_new, int **buffer_sizes_rle, int *nr_blocos){
    unsigned char *buffer_shaf;
    char **codes, *chars, *buffer_new;
    int *tam, i = 0, nr_codes = 0, tam_bloco_new, tam_bloco_shaf;
    *nr_blocos = nrBlocosCod(fp_cod);
    if(buffer_sizes_rle) *buffer_sizes_rle = (int *) malloc((*nr_blocos) * sizeof(int));
    inicializa_arr(&tam, &chars, &codes,256);
    skipNrBlocosShaf(fp_shaf);

    while(i < (*nr_blocos)){
        tamanhoBloco(fp_cod,&tam_bloco_new);
        buffer_new = (char *) malloc(sizeof(char)*tam_bloco_new);
        if(buffer_sizes_rle) (*buffer_sizes_rle)[i] = tam_bloco_new;

        tamanhoBloco(fp_shaf,&tam_bloco_shaf);
        buffer_shaf = (unsigned char *) malloc(sizeof(unsigned char)*tam_bloco_shaf);
        fread(buffer_shaf, sizeof(unsigned char), tam_bloco_shaf, fp_shaf); fseek(fp_shaf,1,SEEK_CUR); /*Dá skip ao '@' para ficar a apontar para o tamanho do prox bloco(n deve haver problema com o EOF)*/
        
        analizaBloco(fp_cod,tam,chars,codes,&nr_codes); /*O fp tem que ficar a apontar para o tamanho do proximo bloco(cuidado quando alterar para ler com buffer)*/
        quickSort(tam,chars,codes,0,nr_codes-1);
        auxDecompressSF(buffer_shaf,buffer_new,chars,codes,nr_codes,&tam_bloco_new);
        fwrite(buffer_new,sizeof(char),tam_bloco_new,fp_new);
        
        free(buffer_new);
        free(buffer_shaf);
        nr_codes = 0;
        i++;
    }
}

void initDecompressRLE(char *path_rle){
    char *path_original, *path_freq;
    removeExtensao(path_rle,&path_original,4);
    substituiExtensao(path_rle,&path_freq,".freq",0);
    FILE *fp_rle      = fopen(path_rle,"rb"),
         *fp_freq     = fopen(path_freq,"r"),
         *fp_original = fopen(path_original,"w");
    int nr_blocos = 0, *buffer_sizes_rle = bufferSizesRLE(fp_freq, &nr_blocos);
    decompressRLE(fp_rle, fp_original, buffer_sizes_rle, &nr_blocos);
    fclose(fp_rle);
    fclose(fp_original);
}

void decompressSF_RLE(char modo, char *path_shaf){
    char *path_new; removeExtensao(path_shaf,&path_new,5);
    char *path_cod; substituiExtensao(path_shaf,&path_cod,".cod",5);
    FILE *fp_shaf  = fopen(path_shaf,"rb"), 
         *fp_cod   = fopen(path_cod,"r"),
         *fp_new   = fopen(path_new,"wb+");
    char check_RLE = checkRLE(fp_cod);
    int nr_blocos = 0;

    if(modo == '0' || check_RLE == 'N') decompressSF(fp_shaf,fp_cod,fp_new,NULL,&nr_blocos);
    else{
        int *buffer_sizes_rle;
        decompressSF(fp_shaf,fp_cod,fp_new,&buffer_sizes_rle,&nr_blocos);
        char *path_original; removeExtensao(path_new,&path_original,4);
        FILE *fp_original = fopen(path_original,"wb");
        fseek(fp_new,0,SEEK_SET);
        decompressRLE(fp_new, fp_original, buffer_sizes_rle, &nr_blocos);
        fclose(fp_original);
    }
    fclose(fp_shaf); 
    fclose(fp_cod); 
    fclose(fp_new); 
}

void moduloD(int argc, char *argv[]){
    if(argc == 4) decompressSF_RLE('1',argv[1]);
    else{/*Caso haja mais do que 5 argumentos, os últimos sáo ignorados"*/
        if(!strcmp(argv[4],"s")) decompressSF_RLE('0',argv[1]);
        else if(!strcmp(argv[4],"r")) initDecompressRLE(argv[1]);
        else printf("Comando inválido!\n");
    }
}

int main(int argc, char *argv[]) {
    if(argc > 1){   
        clock_t startTime = clock();
        if(!strcmp(argv[2],"-m"))
            if(!strcmp(argv[3],"d")) moduloD(argc,argv);
        clock_t finishTime = clock();
        printf("Tempo de execução: %f milisegundos\n", (double)(finishTime - startTime) * 1000 / CLOCKS_PER_SEC );
    }
    else printf("Comando inválido! Utilize o comando \"shafa --help\" para mais informação.\n");
    return 0;
}