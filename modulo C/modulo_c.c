#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int read(char* path,int* tblocos[],char** codes[]){
	FILE *fp;
	char a[64];
	unsigned int nblocos = 0,n;

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
		a[n] = fgetc(fp);
		while (a[n] != '@'){
			n++;
			a[n] = fgetc(fp);
		}
		a[n] = '\0';
		n = atoi(a);
		//printf("%s\n",a);
		(*tblocos)[i] = n;
		//printf("%d\n",i);
		for(int j = 0;j<256;j++){
			n = 0;
			a[n] = fgetc(fp);
			while(a[n] != ';' && a[n] != '@'){
				n++;
				a[n] = fgetc(fp);
			}
			a[n] = '\0';
			//printf("%s\n",a);
			(*codes)[i*256+j] = malloc(sizeof(char)*n);
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
		for (int j = 0;j<256;j++){
			printf("%s\n",c[i*256+j]);
		}
		putc('\n',stdout);
	}

	return 1;
}