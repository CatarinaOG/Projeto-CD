#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void RLEcompression(*f_origin, *f_rle){
	int k= charLeft(0, f_origin);
	int p = 1;
	int rep = 1;
	int block=1;//

	char repch;
	char blockBuffer[66560];

	FILE *fp_origin, *fp_rle;
	fp_origin = fopen(f_origin, 'r');
	fp_rle = fopen(f_rle, 'a');


	while (feof(fp_origin)!){

		k -= (block-1)*65536;

			if( k>66560 ){
				fgets( blockBuffer, 65536, fp_origin);
				repch = blockBuffer[0];

				while(blockBuffer){

					if(blockBuffer[p]==repch && rep < 255) rep++;
					else {
						if(rep>=4) {
							print(fp_rle, char 'rep');
							repch=blockBuffer[p];
							rep=1;
						}
						else{
							while(rep!=0) print(fp_rle, char 'rep');
							repch=blockBuffer[p];
							rep=1;
						}
					};
					p++;
				}
			}
			else{
				fgets( blockBuffer, 66560, fp_origin);
				repch = blockBuffer[0];

				while(blockBuffer){

					if(blockBuffer[p]==repch && rep < 255) rep++;
					else {
						if(rep>=4) {
							print(fp_rle, char 'rep');
							repch=blockBuffer[p];
							rep=1;
						}
						else{
							while(rep!=0) print(fp_rle, char 'rep');
							repch=blockBuffer[p];
							rep=1;
						}
					}
				p++;
				}	
			}
	p=1;
	block++;	
	}
	fclose(f_origin);
	fclose(f_rle);

}