#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read(char* path,int* tblocos[],unsigned char** codes[]){
	FILE *fp;
	char a[64],code;
	unsigned int nblocos = 0,n,tcode,ind,mtcode;

	fp = fopen(path,"r");
	if (!fp) return -1;
	for(int i = 0; i < 3;i++) fgetc(fp);

	a[0] = fgetc(fp);
	while(a[nblocos] != '@'){
		nblocos++;
		a[nblocos] = fgetc(fp);
	}
	a[nblocos] = '\0';
	nblocos = atoi(a);
	//printf("%s |%d\n",a,nblocos);

	*tblocos = malloc(sizeof(int)*nblocos*2);
	*codes = malloc(sizeof(char*)*nblocos*256);
	//tblocos[x+0] = tamnaho do bloco; tblocos[x+1] = o tamanho maximo dos simbolos nesse bloco  
	for(int i = 0;i<nblocos;i++){
		mtcode = 0;
		n = 0;
		a[0] = fgetc(fp);
		while (a[n] != '@'){
			n++;
			a[n] = fgetc(fp);
		}
		a[n] = '\0';
		n = atoi(a);
		//printf("%s\n",a);
		(*tblocos)[i*2] = n;
		//printf("%d\n",i);
		//codes[x][0] = tamanho do simbolo;codes[x][1] = tamanho do ultimo bit;codes[x][y] simbolo 
		for(int j = 0;j<256;j++){
			ind = i * 256;
			tcode = 2;
			code = fgetc(fp);
			a[2] = 0; 
 			for(n = 8;code != ';' && code != '@';n--){
 				if(n == 0){n = 8;a[++tcode] = 0;}
 				a[tcode] = a[tcode] + ((code - '0') << (n-1));
 				code = fgetc(fp);
			}
			if(n != 8) tcode++;
			a[tcode] = '\0';
			a[1] = 8-n;
			a[0] = tcode-2;
			if (tcode > mtcode) mtcode = tcode;
			//printf("%s\n",a);
			(*codes)[ind+j] = malloc(sizeof(char)*tcode);
			strcpy((*codes)[ind+j],a);
		}
		(*tblocos)[i*2+1] = mtcode;
	}

	fclose(fp);
	return nblocos;
}

//preencher letras vs offset ---> ver qual e mais rapido
int makeTable(unsigned char* table,unsigned char* codes[],int tam){
	int n,ares,res;
	tam = tam+2;
	unsigned char *ind,*aind;
	
	for(int i = 0;i<256;i++){
		n = codes[i][0]+1;
		for(int j = 0;j< n;j++)
			table[i*tam+j] = codes[i][j+1];
		for(int j = 1;j<8;j++){
			ind = table+(j*256*tam+i*tam);
			aind = table+((j-1)*256*tam+i*tam);
			ind[0] = (aind[0]+1)%8;
			ares = 0;
			for(int k = 1;k < tam;k++){
				res = aind[k]%2;
				ind[k] = (aind[k] >> 1) + (ares << 7);
				ares = res;
			}
		}
	}
	return 1;
}

int main(){
	int n1,*n = NULL;
	unsigned char **c = NULL,table[256*6*8];

	n1 = read("aaa.txt.cod",&n,&c);;
	makeTable(table,c,4);

	for (int i = 0;i < n1;i++){
		printf("%d|",n[i]);
	}
	putc('\n',stdout);
	for(int i = 0;i<n1;i++){
		for(int j = 0;j<256;j++){
			for(int k = 0;c[i*256+j][k] != '\0';k++){
				printf("%d ",c[i*256+j][k]);	
			}
			putc('\n',stdout);
		}
		putc('\n',stdout);
	}

	for(int i = 0;i < 8;i++){
		for(int j = 0;j < 256;j++){
			printf("off-%d;letra-%5d -->",i,j);
			for(int k = 0;k < 6;k++)
				printf("%d ",table[i*256*6+j*6+k]);
			printf("\n");
		}
	}


	return 1;
}