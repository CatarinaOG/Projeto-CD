#include "modulo_c.h"

void print(char* name,int nblocos,float time,int *tblocos,int *cblocos){
	float taxa = 0;

	printf("Gonçalo Santos, a93279,Tiago Carneiro, a93207, MIEI/CD,\n");
	printf("Módulo: c (codificação dum ficheiro de símbolos)\n");
	printf("Número de blocos: %d\n",nblocos);
	for(int i = 0;i < nblocos; i++){
		printf("Tamanho antes/depois & taxa de Compressão (bloco %d): %d/%d\n",i+1,tblocos[i*2],cblocos[i]);
		taxa +=(float) cblocos[i]/tblocos[i*2]*100;
	}
	printf("Taxa de compressão global: %.0f%\n",(taxa/nblocos));
	printf("Tempo de execução do módulo (milissegundos): %f\n",time);
	printf("Ficheiro gerado: %s\n",name);
}

int read(FILE *fp,int *tblocos,pdarr codes,unsigned char buffer[],int c){
	int tcode,bloco,ind,n,mtcode,tbloco,total = 0,j = 0;
	unsigned char a[64];

	for(tbloco = 0;buffer[c] != '@';c++){
		CBUFFER(c,buffer,fp);
		tbloco = tbloco*10+buffer[c]-'0';
	}
	tblocos[0] = tbloco;
	mtcode = 0;
	codes->n = 0;
	for(int i = 0;i<256;i++){
		tcode = 2;
		a[2] = 0; 
		c++;
 		CBUFFER(c,buffer,fp);
 		for(n = 8;buffer[c] != ';' && buffer[c] != '@';n--){
 			if(n == 0){n = 8;a[++tcode] = 0;}
 			a[tcode] = a[tcode] + ((buffer[c] - '0') << (n-1));
 			c++;
 			CBUFFER(c,buffer,fp);
		}
		if(n == 0){n = 8;a[++tcode] = 0;tcode++;}
		else if(n != 8) tcode++;
		a[tcode] = '\0';
		a[1] = 8-n;
		a[0] = tcode-2;
		if (tcode > mtcode) mtcode = tcode;
		if (a[0] != 0){
			REALLOC_DARR(codes,total,tcode);
			(codes->used)[(codes->n)++] = i;
			for(int j = 0;j<tcode;j++)(codes->arr)[j+total] = a[j];
			//(codes->arr+total)[j] = '\0';
			total += tcode;


		}
	}

	tblocos[1] = mtcode;
	CBUFFER(c,buffer,fp);
	c++;
	CBUFFER(c,buffer,fp);

	return c;	
}

int makeTable(unsigned char* table,pdarr codes,int tam){
	int n,ares,res,total = 0,n1;
	unsigned char *ind,*aind;
	
	for(int i = 0;i<codes->n;i++){
		n1 = codes->used[i];
		n = codes->arr[total]+1;
		for(int j = 0;j<= n;j++)
			table[n1*tam+j] = codes->arr[total++];
		table[n1*tam+n+1] = 0;
		for(int j = 1;j<8;j++){
			ind = table+(j*256*tam+n1*tam);
			aind = table+((j-1)*256*tam+n1*tam);
			if((ind[1] = (aind[1]+1)%8) == 0)
				ind[0] = aind[0] + 1;
			else
				ind[0] = aind[0];
			ares = 0;
			for(int k = 2;k < tam;k++){
				res = aind[k]%2;
				ind[k] = (aind[k] >> 1) + (ares << 7);
				ares = res;
			}
		}
	}
	return 1;
}


int moduloC(char *path){
	unsigned char *name,*in = NULL,*out = NULL,*table = NULL,*line,buffer[BREAD],*pathcod;
	int i,nblocos,*tblocos = NULL,*cblocos = NULL,off,n,tam,c = 0,max = BREAD;
	pdarr codes;
	clock_t t,t1;
	FILE *fp,*fout,*fpcod;

	t = clock();

	for(i = 0;path[i] != '\0';i++);
	CHECK(name = malloc(sizeof(char)*i+6));
	CHECK(pathcod = malloc(sizeof(char)*i+6));

	strcpy(name,path);
	strcpy(name+i,".shaf");
	strcpy(pathcod,path);
	strcpy(pathcod+i,".cod");

	CHECK(fp = fopen(path,"r"));
	CHECK(fout = fopen(name,"w"));
	CHECK(fpcod = fopen(pathcod,"r"));

	fscanf(fpcod,"@%*c@%d@",&nblocos);
	fread(buffer,sizeof(unsigned char),BREAD,fpcod);
	CHECK(tblocos = malloc(sizeof(int)*nblocos*2));
	CHECK(cblocos = malloc(sizeof(int)*nblocos));
	CHECK(in = malloc(sizeof(unsigned char)*max));
	CHECK(out = malloc(sizeof(unsigned char)*max*1.1))
	fprintf(fout,"@%d",nblocos);
	CREATE_DARR(codes);
	
	for(int i = 0;i<nblocos;i++){
		c = read(fpcod,tblocos+2*i,codes,buffer,c);
		if (max < tblocos[i*2]) {
			max = tblocos[i*2];
			free(in);free(out);
			CHECK(in = malloc(sizeof(unsigned char)*max));
			CHECK(out = malloc(sizeof(unsigned char)*max*1.1));
		}

		fread(in,sizeof(unsigned char),tblocos[i*2],fp);
		tam = tblocos[i*2+1] + 3;
		CHECK(table = malloc(sizeof(unsigned char)*256*8*tam));
		makeTable(table,codes,tam);

		off = 0;
		n = 0;
		out[0] = 0;

		for(int j = 0;j<tblocos[i*2];j++){
			line = table+(off*256*tam+in[j]*tam);
			out[n] += line[2];
			for(int k = 1;k<line[0];k++) out[++n] = line[k+2];
			off = line[1];
		}
		out[++n] = '\0';
		cblocos[i] = n;
		fprintf(fout,"@%d@",n);
		fwrite(out,sizeof(unsigned char),n,fout);
		free(table);
	}
	t1 = clock();
	float time = (double)(t1-t) * 1000 / CLOCKS_PER_SEC;
	print(name,nblocos,time,tblocos,cblocos);

	free(in);
	free(out);
	free(name);
	free(pathcod);
	free(tblocos);
	free(cblocos);
	DEL_DARR(codes)
	fclose(fout);
	fclose(fp);
	fclose(fpcod);
	return 1;
}


int main(int argc,char *argv[]){
	moduloC(argv[1]);
	return 1;
}