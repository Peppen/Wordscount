#include "../include/mpi_helper.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void toSend(int *sendCounts, chunk *list, int size, int totalLines, int p) {
  	unsigned long int standardChunkSize = totalLines / p;
  	int remainder = totalLines % p;
  
  	int j = 0;
  	for (int i = 0; i < p; i++) {
    		
    		int chunkToSend = 0;
    		unsigned long int accumulatedChunkSize = 0;
    		unsigned long int nextChunkSize = standardChunkSize;
    		if (remainder > 0) {
      			nextChunkSize++;
      			remainder--;
    		}

    		while (j < size && accumulatedChunkSize < nextChunkSize) {
      			chunkToSend++;
      			accumulatedChunkSize += (list[j].endLine) - (list[j].startLine) + 1;
      			j++;
    		}

    		sendCounts[i] = chunkToSend;
  	}
}
