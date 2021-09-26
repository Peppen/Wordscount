#include "../include/mpi_chunk.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void createChunkDatatype(MPI_Datatype *chunkData) {
  	int blockNumber = 3;
  	int blockLengths[] = {FILE_NAME_SIZE, 1, 1};
  	long int displs[] = {0, FILE_NAME_SIZE, FILE_NAME_SIZE + 8};
  	MPI_Datatype types[] = {MPI_CHAR, MPI_INT64_T, MPI_INT64_T};
  	MPI_Type_create_struct(blockNumber, blockLengths, displs, types, chunkData);
  	MPI_Type_commit(chunkData);
}

int getChunksNumber(counter *chunkList) {
  	int chunkNumber = 0;
  	counter *pointer = chunkList;
  	while(pointer != NULL) {
    		chunkNumber ++;
    		pointer = pointer->next;
  	}
  	return chunkNumber;
}

counter *divideLines(file *fileNames, int fileNumber, int totalLines, int workers) {
  	int standard = totalLines / workers;
  	int remainder = totalLines % workers;

  	int next = standard;
  	if (remainder > 0) {
    		next++;
    		remainder--;
  	}
  
  	file *files = fileNames;
  	counter *count = NULL, *word = NULL;
  	for (int i = 0; i < fileNumber; i++) {
    		int lastLine = -1;
    		int leftLines = files->lineNumber;
    		while (leftLines > 0) {
      			if (word == NULL) {
        			word = (counter *)malloc(sizeof(counter));
        			count = word;
      			} else {
        			word->next = (counter *)malloc(sizeof(counter));
        			word = word->next;
      			}
      			word->fileName = files;
     			word->startLine = lastLine + 1;
      			word->next = NULL;
      
      			int actual;
      			if (leftLines >= next) {
        			actual = next;
        			next = standard;
        			if (remainder > 0) {
          				next++;
          				remainder--;
        			}
      			} else {
        			actual = leftLines;
        			next -= leftLines;
      			}
      			lastLine += actual;
      			word->endLine = lastLine;
      			leftLines -= actual;
    		}
    		files++;
  	}
  	
  	return count;
}

chunk *createChunkList(int chunksNumber, counter *count) {
	chunk *buffer = calloc(chunksNumber, sizeof(chunk));
	counter *pointer = count;
	
	for(int i=0; i<chunksNumber; i++) {
		strcpy(buffer[i].fileName, pointer->fileName->fileName);
    		buffer[i].startLine = pointer->startLine;
    		buffer[i].endLine = pointer->endLine;
    		pointer = pointer->next;
    	}
    	
    	return buffer;

}
