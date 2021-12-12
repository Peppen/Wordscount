#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include "mpi.h"
#include "../include/mpi_info.h"

#define FILE_LOCATION "../files/"
#define MASTER 0

int main(int argc, char *argv[]) {
  	double startTime = MPI_Wtime(), average = 0;
  	int totalLines = 0, totalWords = 0;
  	int rank, p, fileNumber, check = 1, myChunkNumber;
  	int *sending, *receiving;
  	int *displSend, *displRcv;
  	char dirPath[FILE_NAME_SIZE], timelogFilePath[FILE_NAME_SIZE];
  	MPI_Datatype MPI_CHUNK, MPI_WORD;
  	file *fileNames;
  	chunk *chunks, *myChunks;
  	word *gatherMaps, *maps;
  	occurrence *occurrences;

  	MPI_Init(&argc, &argv);
  	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
  	MPI_Comm_size(MPI_COMM_WORLD, &p);
  
  	createChunkDatatype(&MPI_CHUNK);
  	createWordDatatype(&MPI_WORD);

  	strcpy(dirPath, FILE_LOCATION);
  	strcpy(timelogFilePath, TIME_FILE);
  
  	if(rank == MASTER) {
    		printf("Reading files...\n");
    		fileNames = NULL;
    		fileNumber = getFilesName(&fileNames, dirPath);
    		if (fileNames == NULL || fileNumber == 0) {
      			check = 0;
      			printf("No files found in directory %s\n", dirPath);
    		} else {
      			totalLines = getLinesNumber(fileNames, fileNumber, dirPath);
      			printf("Found %d files:\n", fileNumber);
      			file *pointer = fileNames;
      			for (int i = 0; i < fileNumber; i++) {
        			printf("%s has %ld lines\n", pointer->fileName, pointer->lineNumber);
        			pointer++;
      			}
      			printf("Total lines: %d\n", totalLines);
      			printf("Each process will work on %d lines\n\n", totalLines / p);
      			counter *count = divideLines(fileNames, fileNumber, totalLines, p);
      			int size = getChunksNumber(count);
   			chunks = createChunkList(size, count);
      			if(count == NULL) 
      				check = 0;
      			else {
        			sending = (int *)calloc(p, sizeof(int));
        			displSend = (int *)calloc(p, sizeof(int));
        			toSend(sending, chunks, size, totalLines, p);
      			}
    		}
  	}
  
  	if (p != 1) 
    		MPI_Bcast(&check, 1, MPI_INT, 0, MPI_COMM_WORLD);
  	if (check) {
    		MPI_Scatter(sending, 1, MPI_INT, &myChunkNumber, 1, MPI_INT, 0, MPI_COMM_WORLD);
    		myChunks = (chunk *) calloc(myChunkNumber, sizeof(chunk));
    		MPI_Scatterv(chunks, sending, displSend, MPI_CHUNK, myChunks, myChunkNumber, MPI_CHUNK, 0, MPI_COMM_WORLD);
    		count_words(&occurrences, myChunks, myChunkNumber, dirPath);
    		  
    		int myWordNumber = getWordsNumber(occurrences);
    		change(&maps, occurrences);
    
    		if (p != 1) {
      			if (rank == MASTER) {
        			receiving = (int *) calloc(p, sizeof(int));
        			gatherMaps = (word *)calloc(myWordNumber, sizeof(word));
        			displRcv = (int *) calloc(p, sizeof(int));
      			}
      			MPI_Gather(&myWordNumber, 1, MPI_INT, receiving, 1, MPI_INT, 0, MPI_COMM_WORLD);
      			MPI_Gatherv(maps, myWordNumber, MPI_WORD, gatherMaps, receiving, displRcv, MPI_WORD, 0, MPI_COMM_WORLD);
    		}
    
    		double finalTime = MPI_Wtime() - startTime;
    	
    		if (p != 1) {
      			MPI_Reduce(&finalTime, &average, 1, MPI_DOUBLE, MPI_SUM, 0, MPI_COMM_WORLD);
      			average /= p;
    		} else 
      			average = finalTime;
    		

    		if(rank == MASTER) {
    			// Writing words count on file 
      			printf("Writing on file %s\n", FILE_OUTPUT);
      			FILE *f = fopen(FILE_OUTPUT, "w");
      			fprintf(f, "---Collected Data after WordsCount---\n");
      			for (int i = 0; i < myWordNumber; i++) 
			        fprintf(f, "Word: %s -> Occurrence: %ld\n", maps[i].word, maps[i].occurrences);
			// Writing time info on file
      			time_info(p, fileNumber, totalLines, average);
    		}
  	} else {
      		if(rank == MASTER)
    			printf("Aborting...\n");
  	}
  	MPI_Finalize();
  	return 0;
}
