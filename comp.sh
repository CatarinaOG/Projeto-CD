rm  *.o
gcc -c *c */*c -ansi -std=c17 -O3 -lpthread
rm modulo_c.o shafa.o
gcc -c shafa.c -ansi -std=c17 -O3 -lpthread
gcc -o shafa *.o -ansi -std=c17 -O3 -lpthread 