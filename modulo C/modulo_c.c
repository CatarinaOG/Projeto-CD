#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read(char* path,int* tblocos[],char** codes[]){
	FILE *fp;
	char a[64],code;
	unsigned int nblocos = 0,n,tcode;

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

	*tblocos = malloc(sizeof(int)*nblocos);
	*codes = malloc(sizeof(char*)*nblocos*256);
	for(int i = 0;i<nblocos;i++){
		n = 0;
		a[0] = fgetc(fp);
		while (a[n] != '@'){
			n++;
			a[n] = fgetc(fp);
		}
		a[n] = '\0';
		n = atoi(a);
		//printf("%s\n",a);
		(*tblocos)[i] = n;
		//printf("%d\n",i);
		//codes[x][0] = tamanho do ultimo simbolo;codes[x][y] simbolo 
		for(int j = 0;j<256;j++){
			tcode = 1;
			code = fgetc(fp);
			a[1] = 0; 
 			for(n = 0;code != ';' && code != '@';n++){
 				if(n == 8){n = 0;a[++tcode] = 0;}
 				a[tcode] = code - '0' + (a[tcode] << 1);
 				code = fgetc(fp);
			}
			if(n != 0) tcode++;
			a[tcode] = '\0';
			a[0] = n;
			//printf("%s\n",a);
			(*codes)[i*256+j] = malloc(sizeof(char)*tcode);
			strcpy((*codes)[i*256+j],a);

		}
	}

	fclose(fp);
	return nblocos;
}


int main(){
	int n1,*n = NULL;
	char **c = NULL;

	n1 = read("aaa.txt.cod",&n,&c);;

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

	return 1;
}