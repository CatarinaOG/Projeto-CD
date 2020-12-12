#include <stdio.h>
#include <stdlib.h>
#include <string.h>





typedef struct blockfreq {
	int blockSize;
	int freq[255];
	int blockSizeRLE;
	int freqRLE[255];
	struct blockfreq *next;
} *BFreq;



/*
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




void firstBlockCompression(FILE *f_origin, FILE *f_rle){
	int k = charLeft (0, f_origin);
	int p = 1;
	int rep = 1;

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
	int k = charLeft(0, f_origin);  // numero de caracteres
	int p = 1;  // posicao blockBuffer
	int rep = 1;  // repeticoes de um caracter consecutivas
	int block = 1;  // numero do bloco
	
	char repch;   // caracter analizado
	char blockBuffer[66560];  // bloco analisado
	
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
							fprintf(fp_rle, "%c%c%c", 0, repch, rep);
							repch=blockBuffer[p];
							rep=1;
						}
						else{
							for (;rep >= 0; rep--) print(fp_rle, "%c", repch);
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

*/


int charLeft(FILE *fp_origin){
	long int i;
	fseek(fp_origin, 0, SEEK_END);
	i = ftell(fp_origin);
	rewind (fp_origin);
 	return i;
}




int checkCompression (float source,float result){
    float comp;
    int r = 0; 
    comp = (source-result)/source;
    if (comp > 0.05) r = 1;
    return r;
}


int freqFileBuild (BFreq freqList, int block, char *fileName) {
    int i;
    
    FILE *fp_Freq, *fp_FreqRLE;
    
    char fileName2[30];
    
    BFreq pointer;
    
    strcpy(fileName2,fileName);
    fp_Freq = fopen (strcat (fileName2,".freq"), "w");
    
    if (fp_Freq) {
        if (freqList->freqRLE != NULL) fprintf (fp_Freq, "@R@%d@", block);
        else fprintf (fp_Freq, "@N@%d@", block);
        
        for (pointer = freqList; pointer != NULL; pointer = pointer->next) {
            fprintf (fp_Freq, "%d@", pointer->blockSize);
            
            for (i = 0; i < 255; i++) fprintf (fp_Freq, "%d@", pointer->freq);
        }
        fprintf (fp_Freq, "0");
    }
    
    
    if (freqList->freqRLE != NULL) {
        strcpy (fileName2,fileName);
        fp_FreqRLE = fopen (fileName2,".rle.freq" "w");
        
        if (fp_FreqRLE){
            
            fprintf (fp_Freq, "@R@%d@", block);
                    
            for (pointer = freqList; pointer != NULL; pointer = pointer->next) {
                fprintf (fp_FreqRLE, "%d@", pointer->blockSizeRLE);
                
                for (i = 0; i < 255; i++) fprintf (fp_FreqRLE, "%d@", pointer->freqRLE);
            }
            fprintf (fp_FreqRLE, "0");
        }
    }
    
    fclose(fp_FreqRLE);
    fclose(fp_Freq);

    return(0);
}



// esta funcao vai retornar a quantidade de blocos criados
// espaco para se fazer a compressao RLE, escrever no ficheiro e contar os caracteres.... isto se for para ser feita
// caso contrario apenas preencher a lista das frequencias
int RLEcompression (FILE *fp_origin, BFreq *freqList, char *fileName){
	
	BFreq newBFreq = (BFreq) malloc (sizeof (struct blockfreq)); // auxiliar para criar a freqList
	*freqList = newBFreq;
	
	int k = charLeft(fp_origin);  // numero de caracteres
	
	int rep;  // repeticoes de um caracter consecutivas
	char repChar;   // caracter analizado
	
	int block = 0;  // numero do bloco
	int checkCom = 1;  // validade da compressao (0 == valido)
	
	int buffSize; // ultima posicao usada no blockBuffer
	int posBuff;  // posicao blockBuffer
	char auxBuffer [1025];  // auxiliar para o blockBuffer
	char blockBuffer [66561];  // bloco analisado
	
	int posRLE = 0;  // posicao blockRLE
	char blockRLE [66561];  // bloco com o resultado da compressao RLE
	
	int fileIsOpen = 0;
	FILE *fp_RLE;
	
	
	
	fgets (auxBuffer, 1025, fp_origin); // carregar o primeiro KB no auxBuffer
	
	if (feof (fp_origin)){  // comecar a compressao
		do{	
			block++;
			
			//  preparar o blockBuffer
			strcpy (blockBuffer, auxBuffer);
			fgets (blockBuffer + 1023, 64513, fp_origin);    // acrescentar os 63 KB ao blockBuffer (63*1024 + 1) + 1 KB do auxBuffer (0 -> 1023)
			buffSize = 65536;
			
			if (!feof(fp_origin)){  // caso em que e possivel que o fcheiro acabe no proximo KB
				
				fgets (auxBuffer, 1025, fp_origin);
				
				if (feof (fp_origin))  // caso acabe no proximo KB
					strcpy (blockBuffer, auxBuffer);
					buffSize += 1024;
			}
			
			
			
			// caso em que se faz a compressao RLE
			if (checkCom == 0 || block == 1){
				
				rep = 1;
				repChar = blockBuffer[0];
				
				for (posBuff = 1; posBuff < buffSize && posBuff < (k - block*65536); posBuff++){   // ciclo onde executa a compressao
					
					if ((repChar != blockBuffer[posBuff] && (rep > 4 || repchar == 0)) || rep >= 255){  // casos em que se aplica {0}{simbolo}{repeticoes}
						blockRLE[posRLE++] = 0; 
						blockRLE[posRLE++] = repChar;
						blockRLE[posRLE++] = rep;
						rep = 1;
						repChar = blockBuffer[posBuff];
					}
					else if (repChar != blockBuffer[posBuff] && rep < 4) { // caso em que nao ?eficiente fazer a compressao
						for (; rep > 0; rep--, posRLE++) 
							blockRLE[posRLE] = repChar;
						rep = 1;
						repChar = blockBuffer[posBuff];
					}
					else {      // caso em que ha uma repeticao mas que o valor ainda nao atingiu os 255
						rep++;
					}

				}
				
				if (block == 1 && checkCompression(posBuff, posRLE))  // testa se vale a pena aplicar a compressao ao resto do ficheiro
					checkCom = 0;
				
				if (checkCom == 0) {
					// abrir o ficheiro para escrever o resultado da compressao RLE
					if (!fileIsOpen) {
						fileIsOpen = 1;
						FILE *fp_RLE = fopen (fileName, "w");
					}
					
					if (fp_RLE){
						fprintf(fp_RLE, "%s", blockRLE);
						for (int i = 0; i < posBuff; i++) newBFreq->freq[blockBuffer[i]]++;
						newBFreq->blockSizeRLE = posRLE;

					else {
						printf("Can't open %s\n", fileName);
						exit(1);
					}
				}
				
				for (int i = 0; i < posRLE; i++)
					newBFreq->freqRLE[blockRLE[i]]++;
			}
			else 
				newBFreq->freqRLE = NULL;
			
			// falta criar o ficheiro com o resultado do RLE
			// neste ponto as listas blockBuffer e blockRLE tem os caracteres originais e do resultado apenas do bloco
			
			// nao esquecer que mais tarde temos de considerar outros valores para os blocos, e que a compressao pode ser forcada a acontecer
			// fileFreq ainda nao esta feita
			// ja agora..... nao apaguei nada do que estava feito
			
			
			// preenche a lista com as frequencias dos caracteres deste bloco
			newBFreq->blockSize = posBuff;
			
			for (int i = 0; i < posBuff; i++)
				newBFreq->freq[blockBuffer[i]]++;
			
			newBFreq->next = (BFreq) malloc (sizeof (struct blockfreq));
			newBFreq = newBFreq->next;
			
		} while (!foef(fp_origin));
	}
	else {   // contar frequencias nos 1024 iniciais (n? tem tamanho minimo de 1KB)
		
		newBFreq->next = NULL;
		newBFreq->freqRLE = NULL;
		newBFreq->blockSize = k
		
		for (int i = 0; i < k; i++)
			newBFreq->freq[auxBuffer[i]]++;
	}
	
	fclose(fileName);
	
	return block;
}





int main(int argc, char *argv[]){
	int nblocks;
	if (argc > 1){
		
		char f[30];
		strcpy (f, argv[1]);
 		
		FILE *fp_origin, *fp_RLE;
		
		fp_origin = fopen (f, "r");
     	
	    if (fp_origin){
	    	
	    	BFreq freqList;
	    	
	    	nblocks = RLEcompression(fp_origin, &freqList, strcat(f,".rle"));
			strcpy (f, argv[1]); // super importante pois a string f ficou danificada ao ser usada!!!!   ->    cenas.txt -> cenas.txt.rle
	    	
	    	// criar o ficheiro com as frequencias dos caracteres no ficheiro original (?realizado independentemente da situa?o)
			freqFileBuild (&freqList, nblocks ,f);

	    
			fclose(fp_origin);

		    return 0;
		}
	    else{
			printf("Can't open %s\n", f);
			exit(1);
	    }
	    fclose(fp_origin);
	}
	else {
		printf ("Did not enter the location of the destination file\n");
		return 1;
	}
}
