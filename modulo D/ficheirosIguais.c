#include <stdio.h>
#include <string.h>


int main(int argc, char *argv[]){
	
	FILE *f1, *f2;
	char str1[5000], str2[5000];
	int i=0;
	
	
	if (argc != 3){
		printf(" -> Testa se os dois ficheiros fornecidos para a funcao sao identicos. A funcao aceita ser chamada com\n");
		printf("    os argumentos preenchidos ou sem eles. Isto e, a funcao e possivel de ser chamada desta maneira:\n");
		printf("        -> fichirosIguais.exe _____.txt  _____.txt\n\n");
		
		printf(" -> Endereco ficheiro 1:  ");
		gets(str1);
		f1 = fopen (str1, "r");
		printf(" -> Endereco ficheiro 2:  ");
		gets(str1);
		f2 = fopen (str1, "r");
	}
	else{
		f1 = fopen (argv[1], "r");
		f2 = fopen (argv[2], "r");
	}
	
	
	if (f1 && f2){
		
		while (!feof(f1) && !feof(f2)){
			fgets(str1,5000,f1);
			fgets(str2,5000,f2);
			
			if (strcmp(str1,str2)) {
				printf("\ns -> Os ficheiros sao DIFERENTES\n");
				return 1;
			}
		}
		
		if (!feof(f1) || !feof(f2)) {
			printf("\n -> Os ficheiros sao DIFERENTES\n");
			return 1;
		}
		
		printf("\n -> Os ficheiros sao IGUAIS\n");
		
		fclose (f1);
		fclose (f2);
		
		return 0;
	}
	else{
		printf("\n -> !!!!!!  ERRO ao abrir os ficheiros  !!!!!!!\n          (possivel ficheiro inexistente)\n");
	}
}
