#OBJS specifies which files to compile as part of the project
OBJS = modulo_c.o
T_OBJS = thread_modulo_c.o shafa.o
#CC specifies which compiler we're using
CC = gcc

#COMPILER_FLAGS specifies the additional compilation options we're using
# -w suppresses all warnings
COMPILER_FLAGS = -ansi -std=c17 -O2 

#LINKER_FLAGS specifies the libraries we're linking against
LINKER_FLAGS =  -lpthread

#OBJ_NAME specifies the name of our exectuable
OBJ_NAME = test


#This is the target that compiles our executable
%.o:%.c %.h
	$(CC) -c $< $(COMPILER_FLAGS)

shafa : $(T_OBJS)
	$(CC) -o $@ $^ $(COMPILER_FLAGS) $(LINKER_FLAGS) 
	make test

test : $(OBJS)
	$(CC) -o $@ $^ $(COMPILER_FLAGS) $(LINKER_FLAGS) 

clean:
	rm test shafa *.o