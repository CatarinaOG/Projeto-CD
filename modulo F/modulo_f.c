#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>





typedef struct blockfreq {
	int blockSize;
	int freq[255];
	int blockSizeRLE;
	int freqRLE[255];
	struct blockfreq *next;
} *BFreq;


int min (int a, int b){
	return (a>b ? b:a);
}



void freeBFreq(  BFreq t){
	BFreq r;
	while(t!=NULL){
		r = t->next;
		free(t);
		t = r;
	}
}


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
	
    int i, last = -1;
    BFreq pointer;
    
    FILE *fp_Freq, *fp_FreqRLE;
    
    char fileName2[30];
    strcpy(fileName2,fileName);
    
    fp_Freq = fopen (strcat (fileName2,".freq"), "w");
    
	
    if (fp_Freq != NULL) {
        
		fprintf (fp_Freq, "@N@%d@", block);
        
        for (pointer = freqList; pointer != NULL; pointer = pointer->next) {
            fprintf (fp_Freq, "%d@", pointer->blockSize);
            
            for (i = 0, last = -1; i < 255; i++) {
            	
            	if (last != pointer->freq[i]){ 
            		fprintf (fp_Freq, "%d", pointer->freq[i]);
            		last=pointer->freq[i];
            	}
            	
            	if (i < 254 ) fprintf (fp_Freq, ";");
				else fprintf (fp_Freq, "@");
				
			}
        }
        fprintf (fp_Freq, "0");
    }
    else {
		printf("Can't open %s\n", fileName2);
		exit(1);
	}
	
    
    if (freqList->blockSizeRLE != 0) {
        strcpy (fileName2, fileName);

        fp_FreqRLE = fopen (strcat(fileName2,".rle.freq"), "w");
        
        if (fp_FreqRLE != NULL){
            
            fprintf (fp_FreqRLE, "@R@%d@", block);
            
            for (pointer = freqList; pointer != NULL; pointer = pointer->next) {
                fprintf (fp_FreqRLE, "%d@", pointer->blockSizeRLE);
                
                for (i = 0, last = -1; i < 255; i++) {
            		
					if (last != pointer->freqRLE[i]){ 
						fprintf (fp_FreqRLE, "%d", pointer->freqRLE[i]);
						last = pointer->freqRLE[i];
					}
            		
            		if (i < 254 ) fprintf (fp_FreqRLE, ";");
					else fprintf (fp_FreqRLE, "@");
					
				}
            }
            fprintf (fp_FreqRLE, "0");
        }
        else {
			printf("Can't open %s\n", fileName2);
			exit(1);
		}
    }
	
	
    fclose(fp_FreqRLE);
    fclose(fp_Freq);
	
    return 0;
}



// esta funcao vai retornar a quantidade de blocos criados
// espaco para se fazer a compressao RLE, escrever no ficheiro e contar os caracteres.... isto se for para ser feita
// caso contrario apenas preencher a lista das frequencias
int RLEcompression (FILE *fp_origin, BFreq *freqList, char *fileName, int checkCom){
	
	int i;
	
	int rep;			// repeticoes de um caracter consecutivas
	char repChar;		// caracter analizado
	int remainChar;  
	
	int block = 0;						// numero do bloco
	if (checkCom != 0) checkCom = 1;	// validade da compressao (0 == valido)
	
	int buffSize;						// ultima posicao usada no blockBuffer
	int posBuff;  						// posicao blockBuffer
	char blockBuffer [66561];  			// bloco analisado
	
	int auxSize;						// ultima posicao usada no blockBuffer
	char auxBuffer [1025];  			// auxiliar para o blockBuffer
	
	int posRLE = 0;  					// posicao blockRLE
	char blockRLE [66561];  			// bloco com o resultado da compressao RLE
	
	int fileIsOpen = 0; 				// variavel auxiliar que indica se o "ficheiro.rle" ja foi aberto 
	
	FILE *fp_RLE;
	
	auxSize = fread (auxBuffer, sizeof(char), 1024, fp_origin); 	// carregar o primeiro KB no auxBuffer
	
	BFreq newBFreq = (BFreq) malloc (sizeof (struct blockfreq)); 	// auxiliar para criar a freqList
	for (i = 0; i < 255; i++) newBFreq->freq[i] = 0;
	for (i = 0; i < 255; i++) newBFreq->freqRLE[i] = 0;
	
	*freqList = newBFreq;
	
	
	if (auxSize == 1024){  // comecar a compressao caso tenha mais de 1 KB
		
		do{	
			block++;
			
			//  preparar o blockBuffer
			strcpy (blockBuffer, auxBuffer);
			buffSize = auxSize + fread (blockBuffer + 1024, sizeof(char), 64512, fp_origin);    // acrescentar os 63 KB ao blockBuffer (63*1024 + 1) + 1 KB do auxBuffer (0 -> 1023)
			
			if (buffSize == 65536){  // caso em que e possivel que o fcheiro acabe no proximo KB
				
				auxSize = fread (auxBuffer, sizeof(char), 1024, fp_origin);
				
				if (auxSize != 1024)  // caso acabe no proximo KB
					strcpy (blockBuffer + 65536, auxBuffer);
					buffSize += auxSize;
			}
			
			// caso em que se faz a compressao RLE
			if (checkCom == 0 || block == 1){
				
				rep = 1;
				repChar = blockBuffer[0];
				
				for (posBuff = 1; posBuff < buffSize; posBuff++){   // ciclo onde executa a compressao
					
					remainChar = buffSize - posBuff;  // calcula quntos caracteres faltam ate ao final do bloco
					
					if (repChar != blockBuffer[posBuff] || remainChar == 1) {
						
						if (repChar == blockBuffer[posBuff])
							rep++;
						
						if (rep >= 4 || repChar == 0 || rep >= 255 ){
							blockRLE[posRLE++] = 0;			
							blockRLE[posRLE++] = repChar;
							blockRLE[posRLE++] = rep;
						}
						else
							for (; rep > 0; rep--, posRLE++) 
								blockRLE[posRLE] = repChar;
						
						if (remainChar == 1 && repChar != blockBuffer[posBuff]){
							blockRLE[posRLE++] = blockBuffer[posBuff];
						}
						else{ 
							rep = 1;
							repChar = blockBuffer[posBuff];
						}
					}
					else 
						rep++;
				}
				
				
				if (block == 1 && checkCompression(posBuff, posRLE))  // testa se vale a pena aplicar a compressao ao resto do ficheiro
					checkCom = 0;
				
				if (checkCom == 0) {
					// abrir o ficheiro para escrever o resultado da compressao RLE caso o ficheiro ainda nao esteja aberto
					if (!fileIsOpen) {
						fileIsOpen = 1;
						fp_RLE = fopen (fileName, "w");
					}
					
					if (fp_RLE) {
						
						fwrite (blockRLE, sizeof(char), posRLE, fp_RLE);  // escrever o resultado da compressao do bloco no ficheiro RLE
						
						for (i = 0; i < posRLE; i++)  // preencher a lista com as frequencias dos caracteres apos a compressao RLE
							newBFreq->freqRLE[blockRLE[i]]++;
						
						newBFreq->blockSizeRLE = posRLE;  // colocar o tamanho do bloco com o resultado compressao
					}
					else {
						printf("Can't open %s\n", fileName);
						exit(1);
					}
				}
			}
			else 
				newBFreq->blockSizeRLE = 0;
			
			
			// preenche a lista com as frequencias dos caracteres deste bloco
			newBFreq->blockSize = posBuff;
			
			for (i = 0; i < posBuff; i++){
				newBFreq->freq[blockBuffer[i]]++;
			}
			
			if (!feof(fp_origin)){
				newBFreq->next = (BFreq) malloc (sizeof (struct blockfreq));
				
				for (i = 0; i < 255; i++) newBFreq->freq[i] = 0;
				for (i = 0; i < 255; i++) newBFreq->freqRLE[i] = 0;
			}
			else 
				newBFreq->next = NULL;
			
			newBFreq = newBFreq->next;

		} while (!feof(fp_origin));
	}
	else {   // contar frequencias nos 1024 iniciais (n? tem tamanho minimo de 1KB)
		
		newBFreq->next = NULL;
		newBFreq->blockSizeRLE = 0;
		newBFreq->blockSize = buffSize;
		
		for (i = 0; i < buffSize + 1; i++)
			newBFreq->freq[auxBuffer[i]]++;
	}
	
	fclose (fp_RLE);
	
	return block;
}





int main(int argc, char *argv[]){
	
	char str[30] = "aaa.txt";
	
	//printf("say somthing: ");
	//gets(str);
	if (!argc > 1) strcpy (str, argv[1]);
	
	if (strcmp(str, "")){
		
		float clockStart = clock();
	    float clockEnd;
	    float time;
		
		int nblocks;
		
		char fileName[30];
		strcpy (fileName, str);
 		
		FILE *fp_origin;
		
		fp_origin = fopen (fileName, "r");
     	
	    if (fp_origin){
	    	
	    	BFreq freqList;
	    	
printf ("oi oi -> inicio da RLEcompression\n");
	    	nblocks = RLEcompression(fp_origin, &freqList, strcat(fileName,".rle"), 1);
			strcpy (fileName, str); // super importante pois a string f ficou danificada ao ser usada!!!!   ->    cenas.txt -> cenas.txt.rle
printf ("oi oi -> fim da RLEcompression\n\n");
	    	
	    	// criar o ficheiro com as frequencias dos caracteres no ficheiro original (?realizado independentemente da situa?o)
			freqFileBuild (freqList, nblocks, fileName);
			
printf ("oi oi -> noice chegou ao fim\n");
			
			freeBFreq(freqList);
		}
	    else{
			printf("Can't open %s\n", fileName);
			exit(1);
	    }
	    fclose(fp_origin);
		
	    // Reporta o tempo que demorou
	    clockEnd = clock();
	    time = (double)(((clockEnd - clockStart)/CLOCKS_PER_SEC))*1000;
	    
	    printf ("\n Ei princeso demorou %.3f ms\n\t\t.....se fosse a ti trocava por uma torradeira\n", time);
	    return 0;
	}
	else {
		printf ("Did not enter the location of the sorce file\n");
		return 1;
	}

}
