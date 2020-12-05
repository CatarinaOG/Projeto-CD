#include <stdio.h>
#include <stdlib.h>
#include <string.h>


//64*1024=65536
void RLEcompression(*fp_origin, *fp_rle){
	
}


int main(){
    FILE *fp_origin, *fp_RLE, *fp_freq, *fp_RLEfreq;

    RLEcompression(fp_origin, fp_rle);

    freqFile(fp_origin, fp_freq);

    if(checkCompression(fp_freq, fp_RLE)) freqFile(fp_RLE, fp_RLEfreq);
    else ;//delete RlE File

    return 0;
}