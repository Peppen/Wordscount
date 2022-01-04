CC = mpicc

CFLAGS = -c -Wall
SRC := src
NP := 2

all: wordscount

wordscount: main.o mpi_logic.o mpi_file_information.o mpi_chunk.o mpi_info.o
	$(CC) main.o mpi_logic.o mpi_file_information.o mpi_chunk.o mpi_info.o -o wordscount
	
main.o: main.c
	$(CC) $(CFLAGS) main.c

mpi_file_information.o: mpi_file_information.c
	$(CC) $(CFLAGS) mpi_file_information.c

mpi_logic.o: mpi_logic.c
	$(CC) $(CFLAGS) mpi_logic.c

mpi_chunk.o: mpi_chunk.c
	$(CC) $(CFLAGS) mpi_chunk.c
	
mpi_info.o: mpi_info.c
	$(CC) $(CFLAGS) mpi_info.c

clean:
	rm -rf *o wordscount
	
run: all
	mpirun -np $(NP) ./wordscount
	
rerun: all
	mpirun -np 8 --oversubscribe ./wordscount
