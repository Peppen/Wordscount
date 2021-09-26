#include "mpi_file_information.h"
#include "mpi.h"

typedef struct Chunk {
  	char fileName[FILE_NAME_SIZE];
  	unsigned long int startLine;
  	unsigned long int endLine;
} chunk;

typedef struct Counter {
  	file *fileName;
 	int startLine;
  	int endLine;
  	struct Counter *next;
} counter;

counter *divideLines(file *fileNames, int fileNumber, int totalLineNumber, int p);
int getChunksNumber(counter *chunkList);
chunk *createChunkList(int size, counter *chunkList);
void createChunkDatatype(MPI_Datatype *chunkData);
