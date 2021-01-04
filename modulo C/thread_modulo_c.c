#include "thread_modulo_c.h"

int pcount = 0;
pthread_mutex_t mutex;
FILE *fout;

void print(char* name,int nblocos,float time,int *tblocos,int *cblocos){
	float taxa = 0;

	printf("Gonçalo Santos, a93279,Tiago Carneiro, a93207, MIEI/CD,02-01-2021\n");
	printf("Módulo: c (codificação dum ficheiro de símbolos)\n");
	printf("Número de blocos: %d\n",nblocos);
	for(int i = 0;i < nblocos; i++){
		printf("Tamanho antes/depois & taxa de Compressão (bloco %d): %d/%d\n",i+1,tblocos[i*2],cblocos[i]);
		taxa +=1.f-((float) cblocos[i]/tblocos[i*2]);
	}
	printf("Taxa de compressão global: %.0f%\n",(taxa > 0)?(taxa/nblocos)*100:0);
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
		out[n] += line[2];
		for(int k = 1;k<line[0];k++) out[++n] = line[k+2];
		off = line[1];
	}
	out[++n] = '\0';
	arg->cblocos[0] = n;
	free(table);

	while(n != -1){
		pthread_mutex_lock(&mutex);
		if(pcount == arg->bloco){
			fprintf(fout,"@%d@",n);
			fwrite(out,sizeof(unsigned char),n,fout);
			pcount++;
			n = -1;
		}
		pthread_mutex_unlock(&mutex);
	}
}

int moduloC(char *path){
	unsigned char *name,**in = NULL,**out = NULL,*pathcod = NULL,*table = NULL,*line,buffer[BREAD];
	int i,nblocos,*tblocos = NULL,*cblocos = NULL,off,n,tam,c = 0,max[NUM_THREADS];
	clock_t t,t1;
	pdarr codes[NUM_THREADS];
	pthread_t thread[NUM_THREADS];
	ptarg thread_arg[NUM_THREADS];
	FILE *fp,*fpcod;

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
	CHECK(in = malloc(sizeof(unsigned char*)*NUM_THREADS));
	CHECK(out = malloc(sizeof(unsigned char*)*NUM_THREADS));
	CHECK(cblocos = malloc(sizeof(int)*nblocos));
	fprintf(fout,"@%d",nblocos);


	pthread_mutex_init(&mutex,NULL);

	for(int i = 0;i < NUM_THREADS && i<nblocos;i++){
		CHECK(thread_arg[i] = malloc(sizeof(targ)));
		max[i] = BREAD;
		CHECK(in[i] = malloc(sizeof(unsigned char)*max[i]));
		CHECK(out[i] = malloc(sizeof(unsigned char)*max[i]*1.1));
		CREATE_DARR(codes[i]);
	}
	
	for(int i = 0;i<NUM_THREADS;i+=NUM_THREADS){
		for(int j = 0;j<NUM_THREADS && j+i<nblocos;j++){
			n = i+j;
			c = read(fpcod,tblocos+2*n,codes[j],buffer,c);

			if (max[j] < tblocos[n*2]) {
				max[j] = tblocos[n*2];
				free(in[j]);free(out[j]);
				CHECK(in[j] = malloc(sizeof(unsigned char)*max[j]));
				CHECK(out[j] = malloc(sizeof(unsigned char)*max[j]*1.1));
			}
			fread(in[j],sizeof(unsigned char),tblocos[n*2],fp);
			//SET_TARG(thread_arg[j],codes+j,in[j],out[j],tblocos+(n*2),cblocos+n);
			thread_arg[j]->codes = codes[j];
			thread_arg[j]->in = in[j];
			thread_arg[j]->out = out[j];
			thread_arg[j]->tblocos = tblocos+n*2;
			thread_arg[j]->cblocos = cblocos+n;
			thread_arg[j]->bloco = n;

			pthread_create(&(thread[j]),NULL,pencode,thread_arg[j]); 
		}
		for(int j = 0;j<NUM_THREADS && j+i <nblocos;j++) pthread_join(thread[i+j],NULL);
	}
	t1 = clock();
	float time = (double)(t1-t) * 1000 / CLOCKS_PER_SEC;
	print(name,nblocos,time,tblocos,cblocos);

	for(int i = 0;i<nblocos;i++){
		free(thread_arg[i]);
		free(in[i]);
		free(out[i]);
		DEL_DARR(codes[i])
	}
	free(pathcod);
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