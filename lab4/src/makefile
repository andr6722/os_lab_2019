CC=gcc
CFLAGS=-I.

all : parallel_min_max process_memory zombie parallel_sum 

parallel_min_max : utils.o find_min_max.o utils.h find_min_max.h 
	$(CC) -o ./program_2  utils.o find_min_max.o parallel_min_max.c $(CFLAGS)

utils.o : utils.h
	$(CC) -o utils.o -c utils.c $(CFLAGS)
	
process_memory : process_memory.o
	$(CC) -o ./process_memory process_memory.o $(CFLAGS)
	
zombie : zombie.o
	$(CC) -o ./zombie zombie.o $(CFLAGS)
		
process_memory.o : 
	$(CC) -o process_memory.o -c process_memory.c $(CFLAGS) 2>/dev/null >/dev/null

parallel_sum : parallel_sum.o utils.o utils.h  
	$(CC) -o ./psum -pthread utils.o parallel_sum.o $(CFLAGS)
			
clean : 
	rm -rf *.o program_2 zombie process_memory parallel_sum psum 

