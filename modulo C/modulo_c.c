#include "modulo_c.h"
 
void ptable(unsigned char *table,int tam){
	for(int i = 0;i < 8;i++){
		for(int j = 0;j < 256;j++){
			printf("off-%d;letra-%5d -->",i,j);
			for(int k = 0;k < tam;k++)
				printf("%d ",table[i*256*tam+j*tam+k]);
			printf("\n");
		}
	}
}

void pread(int tblocos[],unsigned char* codes[],int nblocos){
	printf("nblocos --> %d\n",nblocos);
	for(int i = 0;i < nblocos*2;i += 2) printf("tbloco --> %d|mbloco --> %d\n",tblocos[i],tblocos[i+1]);
	for(int i = 0;i < nblocos;i++){
		for(int j = 0; j < 256;j++) printf("codigo '%d' --> %s\n",j,codes[i*256+j]);
	}
}

void print(char* name,int nblocos,double time,int *tblocos,int *cblocos){
	double taxa = 0;

	printf("Gonçalo Santos, a93279,Tiago Carneiro, a93207, MIEI/CD,\n");
	printf("Módulo: c (codificação dum ficheiro de símbolos)\n");
	printf("Número de blocos: %d\n",nblocos);
	for(int i = 0;i < nblocos; i++){
		printf("Tamanho antes/depois & taxa de Compressão (bloco %d): %d/%d\n",i+1,tblocos[i*2],cblocos[i]);
		taxa += cblocos[i]/tblocos[i*2];
	}
	printf("Taxa de compressão global: %.0f%\n",taxa/nblocos);
	printf("Tempo de execução do módulo (milissegundos): %f\n",time);
	printf("Ficheiro gerado: %s\n",name);
}

int read1(char* path,int* tblocos[],unsigned char** codes[]){
	int nblocos,tcode,bloco,c,ind,n,mtcode,lim;
	unsigned char buffer[BREAD],a[64];
	FILE *fp;
	
	CHECK(fp = fopen(path,"r"));
	
	fscanf(fp,"@%*c@%d@",&nblocos);
	CHECK(*tblocos = malloc(sizeof(int)*nblocos*2));
	CHECK(*codes = malloc(sizeof(unsigned char*)*nblocos*256));
	fread(buffer,sizeof(unsigned char),BREAD,fp);
	c = 0;
	lim = BREAD;

	for(int i = 0;i < nblocos;i++){
		for(bloco = 0;buffer[c] != '@';c = (c+1)%BREAD)
			bloco = bloco*10+buffer[c]-'0';
		(*tblocos)[i*2] = bloco;
		mtcode = 0;
		for(int j = 0;j<256;j++){
			ind = i * 256;
			tcode = 2;
			a[2] = 0; 
			c = (c+1)%BREAD;
 			for(n = 8;buffer[c] != ';' && buffer[c] != '@';n--){
 				if(n == 0){n = 8;a[++tcode] = 0;}
 				a[tcode] = a[tcode] + ((buffer[c] - '0') << (n-1));
 				c = (c+1)%BREAD;
			}
			if(n != 8) tcode++;
			a[tcode] = '\0';
			a[1] = 8-n;
			a[0] = tcode-2;
			if (tcode > mtcode) mtcode = tcode;
			(*codes)[ind+j] = malloc(sizeof(char)*tcode);
			strcpy((*codes)[ind+j],a);
			if (((lim+BREAD)-c)%BREAD <=272){
				fread(buffer+lim,sizeof(unsigned char),lim%BREAD,fp);
				fread(buffer,sizeof(unsigned char),c,fp);lim = c-1;
			}
		}
		(*tblocos)[i*2+1] = mtcode;
		c = (c+1)%BREAD;
	}
	fclose(fp);
	return nblocos;
}


int read(char* path,int* tblocos[],unsigned char** codes[]){
	FILE *fp;
	char a[64],code;
	unsigned int nblocos = 0,n,tcode,ind,mtcode;

	fp = fopen(path,"r");
	if (!fp) return -1;
	fscanf(fp,"@%*c@%d@",&nblocos);

	//printf("%s |%d\n",a,nblocos);

	CHECK(*tblocos = malloc(sizeof(int)*nblocos*2));
	CHECK(*codes = malloc(sizeof(char*)*nblocos*256));
	//tblocos[x+0] = tamnaho do bloco; tblocos[x+1] = o tamanho maximo dos simbolos nesse bloco  
	for(int i = 0;i<nblocos;i++){
		mtcode = 0;
		code = fgetc(fp);
		for(n = 0;code != '@';code = fgetc(fp))
			n = n*10+code-'0';
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
	tam = tam+3;
	unsigned char *ind,*aind;
	
	for(int i = 0;i<256;i++){
		n = codes[i][0]+1;
		for(int j = 0;j<= n;j++)
			table[i*tam+j] = codes[i][j];
		for(int j = 1;j<8;j++){
			ind = table+(j*256*tam+i*tam);
			aind = table+((j-1)*256*tam+i*tam);
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

int encode(char *path,char  *pathcod){
	unsigned char *name,**codes = NULL,*in = NULL,*out = NULL,*table = NULL,*line;
	int i,nblocos,*tblocos = NULL,*cblocos = NULL,off,n,tam;
	clock_t t;
	FILE *fp,*fout;

	t = clock();

	for(i = 0;path[i] != '\0';i++);
	CHECK(name = malloc(sizeof(char)*i+6));
	if (!name) return -1;

	strcpy(name,path);
	strcpy(name+i,".shaf");

	CHECK(fp = fopen(path,"r"));
	CHECK(fout = fopen(name,"w"));

	nblocos = read1(pathcod,&tblocos,&codes);
	cblocos = malloc(sizeof(int)*nblocos);
	n = tblocos[2*(nblocos-1)];
	if (n < tblocos[0]) n = tblocos[0];
	CHECK(in = malloc(sizeof(unsigned char)*n));
	CHECK(out = malloc(sizeof(unsigned char)*n));

	fprintf(fout,"@%d",nblocos);
	
	for(int i = 0;i<nblocos;i++){
		fread(in,sizeof(unsigned char),tblocos[i*2],fp);
		tam = tblocos[i*2+1] + 3;
		CHECK(table = malloc(sizeof(unsigned char)*256*8*tam));
		makeTable(table,codes,tam-3);

		off = 0;
		n = 0;
		out[0] = 0;
		for(int j = 0;j<tblocos[i*2];j++){
			line = table+(off*256*tam+in[j]*tam);
			//printf("off->%d|n->%d\n",off,n);
			//printf("%c|%d|%d->%s\nout->",in[j],in[j],j,line);
			out[n] += line[2];
			for(int k = 1;k<line[0];k++) out[++n] = line[k+2];
			off = line[1];
			//for(int k = 0;k<=n;k++) printf("%#x ",out[k]);
			//putc('\n',stdout);
		}
		out[++n] = '\0';
		cblocos[i] = n;
		fprintf(fout,"@%d@",n);
		fwrite(out,sizeof(unsigned char),n,fout);
		//putc('@',fout);
		free(table);
	}
	t = (int)(t - clock())/(CLOCKS_PER_SEC/1000);
	print(name,nblocos,t,tblocos,cblocos);

	free(in);
	free(out);
	free(name);
	free(tblocos);
	for(int i = 0;i < nblocos * 256;i++) free(codes[i]);
	free(codes);
	fclose(fout);
	fclose(fp);
}


int main(){
	int n1,*n = NULL,tam = 1;
	clock_t t;
	unsigned char **c = NULL,table[256*(tam+2)*8];
	encode("aaa.txt","aaa.txt.cod");
	/*
	t = clock();
	n1 = read("test.cod",&n,&c);
	t = (clock()-t)/(CLOCKS_PER_SEC/1000000);
	printf("%.9f\n",t);
	//pread(n,c,n1);
	//for(int i = 0;i<10;i++)putc('\n',stdout);
	t = clock();
	n1 = read1("test.cod",&n,&c);
	t = (clock()-t)/(CLOCKS_PER_SEC/1000000);
	printf("%.9f\n",t);
	//pread(n,c,n1);
	*/
	return 1;
}