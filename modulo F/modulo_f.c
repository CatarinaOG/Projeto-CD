#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(){
    RLEcompression();
    freqFile();
    if(checkCompression()) freqFile();
    return 0;
}