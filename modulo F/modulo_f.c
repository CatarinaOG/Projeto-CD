#include <stdio.h>
#include <stdlib.h>
#include <string.h>


// d?0 se a compressao for >5% e 1 se for <5%  (falta testar)
int checkCompression (FILE *f_origin, FILE *f_rle){
	int i,n;
	FILE *fp1, *fp2;
	fp1 = f_origin;
	fp2 = f_rle;
	*fp1 = fopen(file_name, "r");
	*fp2 = fopen(file_name, "r");
	fseek(fp1, 0, SEEK_END);
	fseek(fp2, 0, SEEK_END);
	i = ftell(fp1);
	n = ftell(fp2);
 	return n/i;
}



int charLeft(int n,char *f_origin){
	int i;
	FILE *fp;
	fp = f_origin;
	*fp = fopen(file_name, "r");
	fseek(fp, 0, SEEK_END);
	i = ftell(fp);
 	return i-n;
}



void firstBlockCompression(FILE *f_origin, FILE *f_rle){
	int k = charLeft(0, f_origin);
	int p = 1;
	int rep=1;

	char repch;
	char blockBuffer[66560];

	FILE *fp_origin, *fp_rle;
	fp_origin = fopen(f_origin, 'r');
	fp_rle = fopen(f_rle, 'a');

	if( k>66560 ){
		fgets( blockBuffer, 65536, fp_origin);
		repch = blockBuffer[0];

		while(blockBuffer){

			if(blockBuffer[p]==repch && rep < 255) rep++;
			else {
				if(rep>=4) {
					print(fp_rle, char 'rep');
					repch=blockBuffer[p];
					rep=1;
				}
				else{
					while(rep!=0) print(fp_rle, char 'rep');
					repch=blockBuffer[p];
					rep=1;
				}
			};
			p++;
		}
	}
	else{
		fgets( blockBuffer, 66560, fp_origin);
		repch = blockBuffer[0];

		while(blockBuffer){

			if(blockBuffer[p]==repch && rep < 255) rep++;
			else {
				if(rep>=4) {
					print(fp_rle, char 'rep');
					repch=blockBuffer[p];
					rep=1;
				}
				else{
					while(rep!=0) print(fp_rle, char 'rep');
					repch=blockBuffer[p];
					rep=1;
				}
			};
			p++;
		}	
	}
	fclose(f_origin);
	fclose(f_rle);
}



//64*1024 = 65536
//65536+1024 = 66560 
void RLEcompression(*f_origin, *f_rle){
	int k= charLeft(0, f_origin);
	int p = 1;
	int rep = 1;
	int block=1;//

	char repch;
	char blockBuffer[66560];

	FILE *fp_origin, *fp_rle;
	fp_origin = fopen(f_origin, 'r');
	fp_rle = fopen(f_rle, 'a');


	while (feof(fp_origin)!){

		k -= (block-1)*65536;

			if( k>66560 ){
				fgets( blockBuffer, 65536, fp_origin);
				repch = blockBuffer[0];

				while(blockBuffer){

					if(blockBuffer[p]==repch && rep < 255) rep++;
					else {
						if(rep>=4) {
							print(fp_rle, char 'rep');
							repch=blockBuffer[p];
							rep=1;
						}
						else{
							while(rep!=0) print(fp_rle, char 'rep');
							repch=blockBuffer[p];
							rep=1;
						}
					};
					p++;
				}
			}
			else{
				fgets( blockBuffer, 66560, fp_origin);
				repch = blockBuffer[0];

				while(blockBuffer){

					if(blockBuffer[p]==repch && rep < 255) rep++;
					else {
						if(rep>=4) {
							print(fp_rle, char 'rep');
							repch=blockBuffer[p];
							rep=1;
						}
						else{
							while(rep!=0) print(fp_rle, char 'rep');
							repch=blockBuffer[p];
							rep=1;
						}
					}
				p++;
				}	
			}
	p=1;
	block++;	
	}
	fclose(f_origin);
	fclose(f_rle);

}







// 64*1024=65536
// tive a ver como ?que podera ser melhor estruturar o trabalho
// tenta ver se entendes a logica e se ?possivel fazer assim
// alterei a base da RLEcompression para adaptar ao restante da main... diz-me se consegues adaptar para isto
// esta fun?o recebe um ficheiro original e uma lista para se preencher com frequencias
// devolve o apontador para o ficheiro RLE ou NULL (o nome do ficheiro tem de ser fornecido)
// (o ficheiro tem de estar no inicio quando for devolvido -> rewind)
// maybe poder?trabalhar com strings... nao sei o que estas a pensar fazer
// por agora limita o tamanho a 64KB para facilitar e porque deve ser facil acrescentar esse tipo de coisas
// provavelmente ainda falta coisas em todas as fun?es, e o que est?dever?ser alterado
// n? esquecer que mais tarde temos de considerar outros valores para os blocos, e que a compress? pode ser for?da a acontecer
// checkCompression ainda falta testar como tudo o resto mas devera n? dar erro
// fileFreq ainda n? est?feita
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
	    	
	    	// criar o ficheiro com as frequencias dos caracteres no ficheiro original (?realizado independentemente da situa?o)
			fileFreq (arrayFreq, strcat(f,".freq"));
	    	strcpy (f, argv[1]); // super importante pois a string f ficou danificada ao ser usada!!!!   ->    cenas.txt -> cenas.txt.freq
	    	
	    	
	    	if (fp_RLE) {
	    		// zerar o arrayFreq e preencher novamente com os valores correspondentes ao resultado da compress? RLE
				memset (arrayFreq, 0, 255);
				freqCounter (arrayFreq, fp_RLE);
				// criar o ficheiro com as frequencias dos caracteres ap? a compressao RLE
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
