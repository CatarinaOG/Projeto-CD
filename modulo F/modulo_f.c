#include "modulo_f.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>


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
	
    return (float)(source-result)/source;
}


FFBout freqFileBuild (BFreq freqList, int nblock, char *fileName) {
	
    int i, block, last = -1;
    BFreq pointer;
    
    FILE *fp_Freq, *fp_FreqRLE;
    
    char fileName2[30];
    strcpy(fileName2,fileName);
    
    fp_Freq = fopen (strcat (fileName2,".freq"), "wb");
    
    
    FFBout ffb = (FFBout) malloc(sizeof(struct freqFileBuild_output));
    ffb->arrBlock    = (int*) malloc (sizeof(int) * nblock);
    ffb->arrBlockRLE = (int*) malloc (sizeof(int) * nblock);
    ffb->rle = 0;
	
    if (fp_Freq != NULL) {
        
		fprintf (fp_Freq, "@N@%d@", nblock);
        
        for (pointer = freqList, block = 0; pointer != NULL; pointer = pointer->next, block++) {
            fprintf (fp_Freq, "%d@", pointer->blockSize);
            
            ffb->arrBlock[block] = pointer->blockSize;
            
            for (i = 0, last = -1; i < 256; i++) {
            	
            	if (last != pointer->freq[i]){ 
            		fprintf (fp_Freq, "%d", pointer->freq[i]);
            		last=pointer->freq[i];
            	}
            	
            	if (i < 255 ) fprintf (fp_Freq, ";");
				else fprintf (fp_Freq, "@");
				
			}
        }
        fprintf (fp_Freq, "0");
    }
    else {
		printf("Can't open %s\n", fileName2);
		return NULL;
	}
	
	if (freqList->blockSizeRLE != 0) {
		strcpy (fileName2, fileName);
		
		fp_FreqRLE = fopen (strcat(fileName2,".rle.freq"), "wb");
		
		if (fp_FreqRLE != NULL){
			
            fprintf (fp_FreqRLE, "@R@%d@", nblock);
            ffb->rle = 1;
            
            for (pointer = freqList, block = 0; pointer != NULL; pointer = pointer->next, block++) {
				fprintf (fp_FreqRLE, "%d@", pointer->blockSizeRLE);
				
				ffb->arrBlockRLE[block] = pointer->blockSizeRLE;
				
				for (i = 0, last = -1; i < 256; i++) {
					
					if (last != pointer->freqRLE[i]){ 
						fprintf (fp_FreqRLE, "%d", pointer->freqRLE[i]);
						last = pointer->freqRLE[i];
					}
            		
            		if (i < 255 ) fprintf (fp_FreqRLE, ";");
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
	
    fclose(fp_FreqRLE);
    fclose(fp_Freq);
	
    return ffb;
}




int applyRLECompression (FILE *fp_origin, BFreq *freqList, char *fileName, int checkCom, int blockSizeMultiple, float *compression){
	
	int i;
	
	int rep;					// repeticoes de um caracter consecutivas
	unsigned char repChar;		// caracter analizado
	int remainChar; 
	
	int block = 0;							// numero do bloco
	if(checkCom != 0) checkCom = 1;

	int buffSize;							// ultima posicao usada no blockBuffer
	int posBuff;  							// posicao blockBuffer
	unsigned char *blockBuffer;  					// bloco analisado
	blockBuffer = (unsigned char*) malloc (blockSizeMultiple + 1025);
	
	int auxSize;							// ultima posicao usada no blockBuffer
	char auxBuffer [1025];  				// auxiliar para o blockBuffer
	
	int posRLE = 0; 						// posicao blockRLE
	unsigned char *blockRLE;  						// bloco com o resultado da compressao RLE
	blockRLE = (unsigned char*) malloc (2*(blockSizeMultiple + 1025));
	
	int fileIsOpen = 0; 	// variavel auxiliar que indica se o "ficheiro.rle" ja foi aberto 
	
	FILE *fp_RLE;
	// carregar o primeiro KB no auxBuffer
	auxSize = fread (auxBuffer, sizeof(unsigned char), 1024, fp_origin); 	
	// auxiliar para criar a freqList
	BFreq newBFreq = (BFreq) malloc (sizeof (struct blockfreq)); 	
	for (i = 0; i < 256; i++) { newBFreq->freq[i] = 0; newBFreq->freqRLE[i] = 0; }
	
	*freqList = newBFreq;
	// comecar a compressao caso tenha mais de 1 KB
	if (auxSize == 1024){  
		
		do{
			block++;
			
			// preparar o blockBuffer
			// acrescentar os 63 KB ao blockBuffer (63*1024 + 1) + 1 KB do auxBuffer (0 -> 1023)
			for (i = 0; i < 1024;i++) blockBuffer[i] = auxBuffer[i];
			buffSize = auxSize + fread (&(blockBuffer[1024]), sizeof(unsigned char), blockSizeMultiple - 1024, fp_origin);
			if (buffSize == blockSizeMultiple){  // caso em que e possivel que o fcheiro acabe no proximo KB
				
				auxSize = fread (auxBuffer, sizeof(unsigned char), 1024, fp_origin);
				// caso acabe no proximo KB
				if (auxSize != 1024){  
				for (i = blockSizeMultiple; i < 1024+blockSizeMultiple;i++) blockBuffer[i] = auxBuffer[i];
					buffSize += auxSize;
				}
			}
			
			// caso em que se faz a compressao RLE
			if (checkCom == 0 || block == 1){
				
				rep = 1;
				repChar = blockBuffer[0];
				// calcula quntos caracteres faltam ate ao final do bloco
				remainChar = buffSize;  
				// ciclo onde executa a compressao
				for (posBuff = 1; posBuff < buffSize; posBuff++){   
					
					remainChar--;
					
					if (repChar != blockBuffer[posBuff] || remainChar == 1 || rep == 254) {
						if (repChar == blockBuffer[posBuff])
							rep++;
						
						if (rep >= 4 || repChar == 0){
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
							
							if (rep >= 255){
								rep = 0;
							}
							else{
								rep = 1;
								repChar = blockBuffer[posBuff];
							}
						}
					}
					else 
						rep++;
				}
				// testa se vale a pena aplicar a compressao ao resto do ficheiro
				if (block == 1 && checkCompression(posBuff, posRLE))  
					checkCom = 0;
				else
					newBFreq->blockSizeRLE = 0;
				
				if (checkCom == 0) {
					
					if (!fileIsOpen)
						fp_RLE = fopen (fileName, "wb");
					
					if (fp_RLE) {
						fileIsOpen = 1;
						
						// escrever o resultado da compressao do bloco no ficheiro RLE
						fwrite (blockRLE, sizeof(unsigned char), posRLE, fp_RLE);  
						
						// preencher a lista com as frequencias dos caracteres apos a compressao RLE
						for (i = 0; i < posRLE; i++)
							newBFreq->freqRLE [(unsigned char) blockRLE[i]]++;  
						
						// colocar o tamanho do bloco com o resultado compressao
						newBFreq->blockSizeRLE = posRLE;  
						posRLE = 0;
					}
					else {
						printf("Can't open %s\n", fileName);
						block = -2;
						break;
					}
				}
			}
			else
				newBFreq->blockSizeRLE = 0;
			
			// preenche a lista com as frequencias dos caracteres deste bloco
			newBFreq->blockSize = buffSize;
			
			for (i = 0; i < buffSize; i++){
				newBFreq->freq [(unsigned char) blockBuffer[i]]++;
			}
			
			if (!feof(fp_origin)){
				newBFreq->next = (BFreq) malloc (sizeof (struct blockfreq));
				
				newBFreq = newBFreq->next;
				for (i = 0; i < 256; i++) { newBFreq->freq[i] = 0; newBFreq->freqRLE[i] = 0; }
			}
			else {
				newBFreq->next = NULL;
			}
		} while (!feof(fp_origin));
		if (fileIsOpen){
			*compression = totalCompression (fp_origin, fp_RLE);
			fclose(fp_RLE);
		}
	}
	else {   // contar frequencias nos 1024 iniciais (nao tem tamanho minimo de 1KB)
		
		free(newBFreq);
		printf("O ficheiro n? tem tamanho suficiente.\n");
		return -2;
	}
	
	free(blockBuffer);
	free(blockRLE);
	
	return block;
}


void printModuloF (int block, char *source_file_Name, float time, float compressao, FFBout ffb){
	int i;
	
	printf ("\nDiogo Casal Novo, a88276, Luis Francisco Faria, a93209, MIEI/CD, 1-jan-2021\n");
	printf ("Modulo: f (calculo das frequencias dos simbolos)\n");
	
	printf ("Numero de blocos: %d\n", block);
	printf ("Tamanho dos blocos analisados no ficheiro original: %d", ffb->arrBlock[0]);
	for (i=1; i<block; i++) printf ("/%d", ffb->arrBlock[i]);
	
	if (ffb->rle == 1){
		printf ("\nCompressao RLE: %s%s (%f%c compressao)\n", source_file_Name, ".rle", compressao*100, '%');
		printf ("Tamanho dos blocos analisados no ficheiro RLE: %d", ffb->arrBlockRLE[0]);
		for (i=1; i<block; i++) printf ("/%d", ffb->arrBlockRLE[i]);
	}
	
	
	printf ("\nTempo de execucao do modulo (milissegundos): %f\n", time);
	printf ("Ficheiros gerados: %s%s", source_file_Name, ".freq");
	if (ffb->rle == 1) printf (", %s%s\n", source_file_Name, ".rle.freq");
	printf ("\n");
}


int moduloF (int argc, char **argv){
	
	char *source_file_Name = argv[1];
	
	if (source_file_Name){
		
		int i, arg1 = 1;
		
		float clockStart = clock();
	    float clockEnd;
	    float time;
	    float compression = 0;
		
		int nblocks, blockSizeMultiple = 64*1024;
		FFBout ffb = NULL;
		
		char fileName [30];
		strcpy (fileName, source_file_Name);
		
		FILE *fp_origin;
		fp_origin = fopen (fileName, "rb");
     	
	    if (fp_origin){
	    	BFreq freqList;
	    	
	    	for (i = 2; i < argc; i += 2){
	    		// "-c r"
	    		if(!strcmp(argv[i],"-c") && i+1 < argc && !strcmp(argv[i+1],"r"))		
	    			arg1 = 0;
	    		
	    		if(!strcmp(argv[i],"-b") && i+1 < argc) {
	    			// "-b K"
	    			if(!strcmp(argv[i+1],"K"))				
						blockSizeMultiple = 640*1024;
					
					// "-b m"
		    		if(!strcmp(argv[i+1],"m"))				
						blockSizeMultiple = 8*1024*1024;
					
					// "-b M"
		    		if(!strcmp(argv[i+1],"M"))
						blockSizeMultiple = 64*1024*1024;
				}
			}
			
	    	nblocks = applyRLECompression (fp_origin, &freqList, strcat(fileName,".rle"), arg1, blockSizeMultiple, &compression);
			if (nblocks == -2) return -2;

			// a string fileName ficou danificada ao ser usada   ->    file.txt -> file.txt.rle
			strcpy (fileName, source_file_Name); 
			
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
	    printModuloF (nblocks, source_file_Name, time, compression, ffb);
	    
	    freeFFBout(ffb);
	    
	    return 0;
	}
	else {
		printf ("Did not enter the location of the source file\n");
		return -1;
	}

}




