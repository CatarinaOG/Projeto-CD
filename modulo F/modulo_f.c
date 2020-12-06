#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int charLeft(n, *fp_origin){

}

void firstBlockCompression(f_origin, f_rle){
	int n =0;
	while(feof!){
		
	}
}

//64*1024 = 65536
//65536+1024 = 66560 
void RLEcompression(*f_origin, *f_rle){
	int n;
	int k;
	int block=1;
	int rep=0;
	char repc;

	fopen(f_origin,'r');
	fopen(f_rle,'a');

	if (firstBlockCompression(f_origin, f_rle)!) break;

	while (feof(f_origin)!){

		if(charLeft(n, fp_origin)>66560){
			n=0;
			while(feof(f_origin)! && n<65536) {

				n++;
			}

		}
		else{

		}
	block++;
	}

}


int main(){
    FILE *f_origin, *f_RLE, *f_freq, *f_RLEfreq;

    RLEcompression(f_origin, f_rle);

    freqFile(f_origin, f_freq);

    if(checkCompression(f_freq, f_RLE)) freqFile(f_RLE, f_RLEfreq)
    else ;//delete RlE File

    return 0;
}