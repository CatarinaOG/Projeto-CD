//#include "modulo_f.h"
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

typedef struct freqFileBuild_output {
	int rle;
	int *arrBlock;
	int *arrBlockRLE;
} *FFBout;




void freeFFBout (FFBout t){
	free(t->arrBlock);
	free(t->arrBlockRLE);
	free(t);
}


void freeBFreq (BFreq t){
	BFreq r;
	while(t!=NULL){
		r = t->next;
		free(t);
		t = r;
	}
}


int checkCompression (float source,float result){
    float comp;
    int r = 0; 
    comp = (source-result)/source;
    if (comp > 0.05) r = 1;
    return r;
}


float totalCompression (FILE *fp_origin, FILE *fp_RLE){
	float source, result;
	
	fseek(fp_origin, 0, SEEK_END);
	source = ftell(fp_origin);
	
	fseek(fp_RLE, 0, SEEK_END);
	result = ftell(fp_RLE);
	
    return (source-result)/source;
}



FFBout freqFileBuild (BFreq freqList, int nblock, char *fileName) {
	
    int i, block, last = -1;
    BFreq pointer;
    
    FILE *fp_Freq, *fp_FreqRLE;
    
    char fileName2[30];
    strcpy(fileName2,fileName);
    
    fp_Freq = fopen (strcat (fileName2,".freq"), "w");
    
    FFBout ffb = (FFBout) malloc(sizeof(struct freqFileBuild_output));
    ffb->arrBlock    = (int*) malloc (sizeof(int) * nblock);
    ffb->arrBlockRLE = (int*) malloc (sizeof(int) * nblock);
    ffb->rle = 0;
	
    if (fp_Freq != NULL) {
        
		fprintf (fp_Freq, "@N@%d@", nblock);
        
        for (pointer = freqList, block = 0; pointer != NULL; pointer = pointer->next, block++) {
            fprintf (fp_Freq, "%d@", pointer->blockSize);
            
            ffb->arrBlock[block] = pointer->blockSize;
            
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
		return NULL;
	}
	
	
for (i = 0; i < block; i++) printf (" Bloco %d de %d: %d\n", i+1, block, ffb->arrBlock[i]);
	
	if (freqList->blockSizeRLE != 0) {
		strcpy (fileName2, fileName);
		
		fp_FreqRLE = fopen (strcat(fileName2,".rle.freq"), "w");
		
		if (fp_FreqRLE != NULL){
			
            fprintf (fp_FreqRLE, "@R@%d@", nblock);
            ffb->rle = 1;
            
            for (pointer = freqList, block = 0; pointer != NULL; pointer = pointer->next, block++) {
				fprintf (fp_FreqRLE, "%d@", pointer->blockSizeRLE);
				
				ffb->arrBlockRLE[block] = pointer->blockSizeRLE;
				
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
			return NULL;
		}
    }

for (i = 0; i < block; i++) printf (" Bloco %d de %d: %d\n", i+1, block, ffb->arrBlockRLE[i]);
	
    fclose(fp_FreqRLE);
    fclose(fp_Freq);
	
    return ffb;
}




int applyRLECompression (FILE *fp_origin, BFreq *freqList, char *fileName, int checkCom, int blockSizeMultiple, float *compression){
	
	int i;
	
	int rep;			// repeticoes de um caracter consecutivas
	char repChar;		// caracter analizado
	int remainChar;  
	
	int block = 0;									// numero do bloco
	if (checkCom != 0) checkCom = 1;				// validade da compressao (0 == valido)
	
	int buffSize;									// ultima posicao usada no blockBuffer
	int posBuff;  									// posicao blockBuffer
	char blockBuffer [blockSizeMultiple + 1025];  	// bloco analisado
	
	int auxSize;									// ultima posicao usada no blockBuffer
	char auxBuffer [1025];  						// auxiliar para o blockBuffer
	
	int posRLE = 0; 								// posicao blockRLE
	char blockRLE [blockSizeMultiple + 1025];  		// bloco com o resultado da compressao RLE
	
	int fileIsOpen = 0; 	// variavel auxiliar que indica se o "ficheiro.rle" ja foi aberto 
	
	FILE *fp_RLE;
	
	auxSize = fread (auxBuffer, sizeof(char), 1024, fp_origin); 	// carregar o primeiro KB no auxBuffer
	
	BFreq newBFreq = (BFreq) malloc (sizeof (struct blockfreq)); 	// auxiliar para criar a freqList
	for (i = 0; i < 255; i++) newBFreq->freq[i] = 0;
	for (i = 0; i < 255; i++) newBFreq->freqRLE[i] = 0;
	
	*freqList = newBFreq;
	
	
	if (auxSize == 1024){  // comecar a compressao caso tenha mais de 1 KB
		
		do{	
			block++;
			
			// preparar o blockBuffer
			// acrescentar os 63 KB ao blockBuffer (63*1024 + 1) + 1 KB do auxBuffer (0 -> 1023)
			strcpy (blockBuffer, auxBuffer);
			buffSize = auxSize + fread (blockBuffer + 1024, sizeof(char), blockSizeMultiple - 1024, fp_origin);
			
			if (buffSize == blockSizeMultiple){  // caso em que e possivel que o fcheiro acabe no proximo KB
				
				auxSize = fread (auxBuffer, sizeof(char), 1024, fp_origin);
				
				if (auxSize != 1024)  // caso acabe no proximo KB
					strcpy (blockBuffer + blockSizeMultiple, auxBuffer);
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
						return -2;
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
		
		if (fileIsOpen) *compression = totalCompression (fp_origin, fp_RLE);
	}
	else {   // contar frequencias nos 1024 iniciais (nao tem tamanho minimo de 1KB)
		
		newBFreq->next = NULL;
		newBFreq->blockSizeRLE = 0;
		newBFreq->blockSize = buffSize;
		
		for (i = 0; i < buffSize + 1; i++)
			newBFreq->freq[auxBuffer[i]]++;
	}
	
	fclose (fp_RLE);
	
	return block;
}




void printModuloF (int block, char *sorce_file_Name, float time, float compressao, FFBout ffb){
	int i;
	
	printf ("\nDiogo Casal Novo, a88276, Luis Francisco Faria, a93209, MIEI/CD, 1-jan-2021\n");
	printf ("Modulo: f (calculo das frequencias dos simbolos)\n");
	
	printf ("Numero de blocos: %d\n", block);
	printf ("Tamanho dos blocos analisados no ficheiro original: %d", ffb->arrBlock[0]);
	for (i=1; i<block; i++) printf ("/%d", ffb->arrBlock[i]);
	
	if (ffb->rle == 1){
		printf ("\nCompressao RLE: %s%s (%f%c compressao)\n", sorce_file_Name, ".rle", compressao*100, '%');
		printf ("Tamanho dos blocos analisados no ficheiro RLE: %d", ffb->arrBlockRLE[0]);
		for (i=1; i<block; i++) printf ("/%d", ffb->arrBlockRLE[i]);
	}
	
	
	printf ("\nTempo de execucao do modulo (milissegundos): %f\n", time);
	printf ("Ficheiros gerados: %s%s", sorce_file_Name, ".rle");
	if (ffb->rle == 1) printf (", %s%s\n", sorce_file_Name, ".rle.freq");
}



// arg1: "-c r"      -> 0| 1(std)  (possibilidade de obrigatoriamente realizar a compressao)
// arg2: "-b K|m|M"  -> 1(640Kb) | 2(8Mb) | 3(64Mb)
int rleCompression (char *sorce_file_Name, int arg1, int arg2){
	if (sorce_file_Name){
		
		float clockStart = clock();
	    float clockEnd;
	    float time;
	    float compression = 0;
		
		int nblocks, blockSizeMultiple = 64*1024;
		FFBout ffb = NULL;
		
		char fileName [30];
		strcpy (fileName, sorce_file_Name);
		
		FILE *fp_origin;
		fp_origin = fopen (fileName, "r");
     	
	    if (fp_origin){
	    	BFreq freqList;
	    	
	    	if (arg2 < 0)
		    	switch(arg2){	// caso em que e exigido um tamanho especifico nos blocos "-b K|m|M"
		    		case 1:
		    			blockSizeMultiple = 640*1024; 		// "-b K"
		    			break;
		    		case 2:
		    			blockSizeMultiple = 8*1024*1024;	// "-b m"
		    			break;
		    		case 3:
		    			blockSizeMultiple = 64*1024*1024;	// "-b M"
		    			break;
				}
	    	
	    	nblocks = applyRLECompression (fp_origin, &freqList, strcat(fileName,".rle"), arg1, blockSizeMultiple, &compression);
			if (nblocks == -2) return -2;
			
			strcpy (fileName, sorce_file_Name); // super importante pois a string fileName ficou danificada ao ser usada!!!!   ->    file.txt -> file.txt.rle
			
	    	// criar o ficheiro com as frequencias dos caracteres no ficheiro original (e realizado independentemente da situacao)
			ffb = freqFileBuild (freqList, nblocks, fileName);
			if (ffb == NULL) return -2;
			
			freeBFreq(freqList);
		}
	    else{
			printf("Can't open %s\n", fileName);
			return -2;
	    }
	    fclose(fp_origin);
		
	    // Reporta o tempo que demorou
	    clockEnd = clock();
	    time = ((clockEnd - clockStart)/CLOCKS_PER_SEC)*1000;
	    
	    printModuloF (nblocks, sorce_file_Name, time, compression, ffb);
	    
	    freeFFBout(ffb);
	    return 0;
	}
	else {
		printf ("Did not enter the location of the sorce file\n");
		return -1;
	}

}


int main (int argC, char *argV[]) {
	int r = rleCompression ("aaa.txt", 1, 0);
	printf ("\n\n--------------------------------\n valor retornado: %d",r);
	return r;
}




