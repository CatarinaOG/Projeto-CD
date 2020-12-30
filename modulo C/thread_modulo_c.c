#include "thread_modulo_c.h"
 
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

int read1(FILE *fp,int *tblocos,pdarr codes,unsigned char buffer[],int c){
	int tcode,bloco,ind,n,mtcode,tbloco,total = 0,j = 0;
	unsigned char a[64];

	for(tbloco = 0;buffer[c] != '@';c++){
		CBUFFER(c,buffer,fp);
		tbloco = tbloco*10+buffer[c]-'0';
	}
	//printf("tbloco->%d\n",tbloco);
	tblocos[0] = tbloco;
	mtcode = 0;
	codes->n = 0;
	for(int i = 0;i<256;i++){
		tcode = 2;
		a[2] = 0; 
		c++;
 		for(n = 8;buffer[c] != ';' && buffer[c] != '@';n--){
 			CBUFFER(c,buffer,fp);
 			if(n == 0){n = 8;a[++tcode] = 0;}
 			a[tcode] = a[tcode] + ((buffer[c] - '0') << (n-1));
 			c++;
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

			/*
			printf("a-->");
			for(int k = 0;k < tcode;k++)
				printf("%d,",a[k]);
			printf("\ntcode-->%d\ncodes -->",tcode);
			for(int k = 0; k < total;k++) printf("%d,",codes->arr[k]);
			printf("\nused-->");
			for(int k = 0;k < codes->n;k++) printf("%d,",codes->used[k]);
			printf("\n");*/
		}
	}
	tblocos[1] = mtcode;
	CBUFFER(c,buffer,fp);
	c++;

	return c;	
}

/*
int read1(char* path,int* tblocos[],unsigned char** codes[]){
	int nblocos,tcode,bloco,c,ind,n,mtcode;
	unsigned char buffer[BREAD],a[64];
	FILE *fp;
	//clock_t t,t1;
	//t = clock();
	
	CHECK(fp = fopen(path,"r"));
	
	fscanf(fp,"@%*c@%d@",&nblocos);
	CHECK(*tblocos = malloc(sizeof(int)*nblocos*2));
	CHECK(*codes = malloc(sizeof(unsigned char*)*nblocos*256));
	fread(buffer,sizeof(unsigned char),BREAD,fp);
	c = 0;

	for(int i = 0;i < nblocos;i++){
		for(bloco = 0;buffer[c] != '@';c++){
			CBUFFER(c,buffer,fp);
			bloco = bloco*10+buffer[c]-'0';
		}
		(*tblocos)[i*2] = bloco;
		mtcode = 0;
		for(int j = 0;j<256;j++){
			ind = i * 256;
			tcode = 2;
			a[2] = 0; 
			c++;
 			for(n = 8;buffer[c] != ';' && buffer[c] != '@';n--){
 				CBUFFER(c,buffer,fp);
 				if(n == 0){n = 8;a[++tcode] = 0;}
 				a[tcode] = a[tcode] + ((buffer[c] - '0') << (n-1));
 				c++;
			}
			if(n != 8) tcode++;
			a[tcode] = '\0';
			a[1] = 8-n;
			a[0] = tcode-2;
			if (tcode > mtcode) mtcode = tcode;
			(*codes)[ind+j] = malloc(sizeof(char)*tcode);
			strcpy((*codes)[ind+j],a);
		}
		(*tblocos)[i*2+1] = mtcode;
		CBUFFER(c,buffer,fp);
		c++;
	}
	fclose(fp);
	//t1 = clock();
	//printf("time read->%f\n",(float) (t1-t)*1000/CLOCKS_PER_SEC);
	return nblocos;
}*/

/*
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
}*/

/*
//preencher letras vs offset ---> ver qual e mais rapido
int makeTable(unsigned char* table,unsigned char* codes[],int tam){
	int n,ares,res;
	unsigned char *ind,*aind;
	//clock_t t,t1;
	//t = clock();
	
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
	//t1 = clock();
	//printf("time make table->%f\n",(float) (t1-t)*1000/CLOCKS_PER_SEC);
	return 1;
}*/

int makeTable(unsigned char* table,pdarr codes,int tam){
	int n,ares,res,total = 0,n1;
	unsigned char *ind,*aind;
	//clock_t t,t1;
	//t = clock();
	
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
	//t1 = clock();
	//printf("time make table->%f\n",(float) (t1-t)*1000/CLOCKS_PER_SEC);
	//ptable(table,tam);
	return 1;
}

/*
float encode(char *path,char  *pathcod){
	unsigned char *name,**codes = NULL,*in = NULL,*out = NULL,*table = NULL,*line;
	int i,nblocos,*tblocos = NULL,*cblocos = NULL,off,n,tam;
	clock_t t,t1;
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
	t1 = clock();
	float time = (double)(t1-t) * 1000 / CLOCKS_PER_SEC;
	//print(name,nblocos,time,tblocos,cblocos);

	free(in);
	free(out);
	free(name);
	free(tblocos);
	free(cblocos);
	for(int i = 0;i < nblocos * 256;i++) free(codes[i]);
	free(codes);
	fclose(fout);
	fclose(fp);
	return time;
}*/

void pencode(ptarg arg){
	unsigned char *out = arg->out,*in = arg->in,*line,*table = NULL;
	int tam = arg->tblocos[1] + 3,off,n;
	CHECK(table = malloc(sizeof(unsigned char)*256*8*tam));
	makeTable(table,arg->codes,tam);

	off = 0;
	n = 0;
	out[0] = 0;
	for(int j = 0;j < arg->tblocos[0];j++){
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
	arg->cblocos[0] = n;
	free(table);
}

float encode(char *path,char  *pathcod){
	unsigned char *name,**in = NULL,**out = NULL,*table = NULL,*line,buffer[BREAD];
	int i,nblocos,*tblocos = NULL,*cblocos = NULL,off,n,tam,c = 0,max[NUM_THREADS];
	pdarr codes[NUM_THREADS];
	clock_t t,t1;
	FILE *fp,*fout,*fpcod;
	pthread_t thread[NUM_THREADS];
	ptarg thread_arg[NUM_THREADS];

	t = clock();

	for(i = 0;path[i] != '\0';i++);
	CHECK(name = malloc(sizeof(char)*i+6));
	if (!name) return -1;

	strcpy(name,path);
	strcpy(name+i,".shaf");

	CHECK(fp = fopen(path,"r"));
	CHECK(fout = fopen(name,"w"));
	CHECK(fpcod = fopen(pathcod,"r"));

	fscanf(fpcod,"@%*c@%d@",&nblocos);
	fread(buffer,sizeof(unsigned char),BREAD,fpcod);
	CHECK(tblocos = malloc(sizeof(int)*nblocos*2));
	CHECK(in = malloc(sizeof(unsigned char*)*NUM_THREADS));
	CHECK(out = malloc(sizeof(unsigned char*)*NUM_THREADS));
	CHECK(cblocos = malloc(sizeof(int)*nblocos));
	fprintf(fout,"@%d",nblocos);

	for(int i = 0;i < NUM_THREADS && i<nblocos;i++){
		CHECK(thread_arg[i] = malloc(sizeof(targ)));
		max[i] = BREAD;
		CHECK(in[i] = malloc(sizeof(unsigned char)*max[i]));
		CHECK(out[i] = malloc(sizeof(unsigned char)*max[i]));
		CREATE_DARR(codes[i]);
	}
	
	for(int i = 0;i<nblocos;i+=NUM_THREADS){
		for(int j = 0;j<NUM_THREADS && j+i<nblocos;j++){
			n = i+j;
			c = read1(fpcod,tblocos+2*n,codes[j],buffer,c);

			if (max[j] < tblocos[n*2]) {
				max[j] = tblocos[n*2];
				free(in[j]);free(out[j]);
				CHECK(in[j] = malloc(sizeof(unsigned char)*max[j]));
				CHECK(out[j] = malloc(sizeof(unsigned char)*max[j]));
			}
			fread(in[j],sizeof(unsigned char),tblocos[n*2],fp);
			//SET_TARG(thread_arg[j],codes+j,in[j],out[j],tblocos+(n*2),cblocos+n);
			thread_arg[j]->codes = codes[j];
			thread_arg[j]->in = in[j];
			thread_arg[j]->out = out[j];
			thread_arg[j]->tblocos = tblocos+n*2;
			thread_arg[j]->cblocos = cblocos+n;

			pthread_create(&(thread[j]),NULL,pencode,thread_arg[j]); 
		}
		for(int j = 0;j<NUM_THREADS && j+i<nblocos;j++){
			pthread_join(thread[j],NULL);
			fprintf(fout,"@%d@",cblocos[i+j]);
			fwrite(out[j],sizeof(unsigned char),cblocos[i+j],fout);
		}


		//putc('@',fout);
	}
	t1 = clock();
	float time = (double)(t1-t) * 1000 / CLOCKS_PER_SEC;
	print(name,nblocos,time,tblocos,cblocos);

	for(int i = 0;i<NUM_THREADS;i++){
		free(in[i]);
		free(out[i]);
		//DEL_DARR(codes[i])
	}
	free(in);
	free(out);
	free(name);
	free(tblocos);
	free(cblocos);
	fclose(fout);
	fclose(fp);
	fclose(fpcod);

	return time;
}


int main(){
	int n1,*n = NULL,tam = 1;
	float total = 0;
	unsigned char **c = NULL,table[256*(tam+2)*8];
	//for (int i = 0;i < 1000;i++)
		total += encode("aaa.txt.rle","aaa.txt.rle.cod");
	//printf("%f\n",total/1000);
	return 1;
}