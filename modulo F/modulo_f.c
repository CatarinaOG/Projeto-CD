#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// dá 0 se a compressao for >5% e 1 se for <5%  (falta testar)
int checkCompression (char* str1, char* str2){
    float comp, i = strlen(str1), f = strlen(str2);
    int r = 1, ; 
    comp = (i-f)/i;
    if (comp > 0.05) r = 0;
    return r;
}



int charLeft(n, *fp_origin){

}

void firstBlockCompression(f_origin, f_rle){
	int n =0;
	while(feof!){
		
	}
}

//64*1024 = 65536
//65536+1024 = 66560 
void RLEcompression(*f_origin, *f_rle){
	int n;
	int k;
	int block=1;
	int rep=0;
	char repc;

	fopen(f_origin,'r');
	fopen(f_rle,'a');

	if (firstBlockCompression(f_origin, f_rle)!) break;

	while (feof(f_origin)!){

		if(charLeft(n, fp_origin)>66560){
			n=0;
			while(feof(f_origin)! && n<65536) {

				n++;
			}

		}
		else{

		}
	block++;
	}

}






// 64*1024=65536
// tive a ver como é que podera ser melhor estruturar o trabalho
// tenta ver se entendes a logica e se é possivel fazer assim
// alterei a base da RLEcompression para adaptar ao restante da main... diz-me se consegues adaptar para isto
// esta função recebe um ficheiro original e uma lista para se preencher com frequencias
// devolve o apontador para o ficheiro RLE ou NULL (o nome do ficheiro tem de ser fornecido)
// (o ficheiro tem de estar no inicio quando for devolvido -> rewind)
// maybe poderá trabalhar com strings... nao sei o que estas a pensar fazer
// por agora limita o tamanho a 64KB para facilitar e porque deve ser facil acrescentar esse tipo de coisas
// provavelmente ainda falta coisas em todas as funções, e o que está deverá ser alterado
// não esquecer que mais tarde temos de considerar outros valores para os blocos, e que a compressão pode ser forçada a acontecer
// checkCompression ainda falta testar como tudo o resto mas devera não dar erro
// fileFreq ainda não está feita
// ja agora..... tens duas defenicoes da RLEcompression porque nao apaguei nada do que estava feito
FILE* RLEcompression (FILE *fp_origin, int *arrayFreq, char *fileName){
	
	FILE *fp_RLE = fopen (fileName, "w")
	
	// abrir o ficheiro para escrever o resultado da compressao RLE
	if (fp_RLE){
		// espaco para se fazer a compressao RLE, escrever no ficheiro e contar os caracteres.... isto se for para ser feita
		// caso contrario apenas preencher a lista das frequencias
		
		
		
		return rewind(fp_RLE);
	}
	else {
		printf("Can't open %s\n", fileName);
		exit(1);
	}
}








// a partir de um array de 255 inteiros cria um ficheiro com as frequencias dos caracteres (o nome do ficheiro tem de ser fornecido)
int* fileFreq (int *arrayFreq, char *fileName){
	// abrir o ficheiro para escrever frequencias
	f = fopen (fileName, "w");
	if (f){
		// espaco para se escrever no ficheiro (ficheiro limpo)
		
		
		
		
		
	}
	else{
		printf("Can't open %s\n", fileName);
		exit(1);
    }
    
	fclose(f);
}






int main(int argc, char *argv[]){
	
	if (argc > 1){
		
		char f[30];
		strcpy (f, argv[1]);
 		
		FILE *fp_origin, *fp_RLE;
		
		fp_origin = fopen (f, "r");
     	
	    if (fp_origin){
	    	
	    	int *arrayFreq[255] = {0};
	    	
	    	fp_RLE = RLEcompression(fp_origin, arrayFreq, strcat(f,".rle"));
			strcpy (f, argv[1]); // super importante pois a string f ficou danificada ao ser usada!!!!   ->    cenas.txt -> cenas.txt.rle
	    	
	    	// criar o ficheiro com as frequencias dos caracteres no ficheiro original (é realizado independentemente da situação)
			fileFreq (arrayFreq, strcat(f,".freq"));
	    	strcpy (f, argv[1]); // super importante pois a string f ficou danificada ao ser usada!!!!   ->    cenas.txt -> cenas.txt.freq
	    	
	    	
	    	if (fp_RLE) {
	    		// zerar o arrayFreq e preencher novamente com os valores correspondentes ao resultado da compressão RLE
				memset (arrayFreq, 0, 255);
				freqCounter (arrayFreq, fp_RLE);
				// criar o ficheiro com as frequencias dos caracteres após a compressao RLE
	    		fileFreq (arrayFreq, strcat(f,".rle.freq"));
			}
	    	
		    
			fclose(fp_origin);
			fclose(fp_RLE);
		    return 0;
		}
	    else{
			printf("Can't open %s\n", f);
			exit(1);
	    }
	else {
		printf ("Did not enter the location of the destination file\n");
		return 1;
	}
}
