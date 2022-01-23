#include "../include/mpi_logic.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


void createWordDatatype(MPI_Datatype *wordData) {
	int blockNumber = 2;
	int blockLengths[] = {LINE_LIMIT, 1};
	long int displs[] = {0, LINE_LIMIT};
        MPI_Datatype types[] = {MPI_CHAR, MPI_INT64_T};
        MPI_Type_create_struct(blockNumber, blockLengths, displs, types, wordData);
        MPI_Type_commit(wordData);
}

void toLower(char *s) {
     	int i;
     	for (i=0; i<strlen(s); i++) {
          	if ( (s[i]>='A') && (s[i]<='Z') )
               s[i]+=32;
     }
}

void del(char *line) {
     	for (int i = 0, j; line[i] != '\0'; ++i) {
      		while (!(line[i] >= 'a' && line[i] <= 'z') && !(line[i] >= 'A' && line[i] <= 'Z') && !(line[i] == '\0')) {
         		for (j = i; line[j] != '\0'; ++j) {
            			line[j] = line[j + 1];
         		}
         		line[j] = '\0';
      		}
   	}
}

int getWordsNumber(occurrence *words) {
  	int wordsNumber = 0;
  	occurrence *pointer = words;
  	while (pointer != NULL) {
    		wordsNumber++;
    		pointer = pointer->next;
  	}
  	return wordsNumber;
}

void count_words(occurrence **maps, chunk *chunks, int myChunkNumber, char *dirPath) {
  	*maps = NULL;
  	char wordBuf[LINE_LIMIT];

  	for (int i = 0; i < myChunkNumber; i++) {
    		char *fileFullName = (char *)calloc(strlen(dirPath) + strlen(chunks[i].fileName) + 1, sizeof(char));
    		strcpy(fileFullName, dirPath);
    		strcat(fileFullName, chunks[i].fileName);
    		FILE *fp = fopen(fileFullName, "r");

    		for (int j = 0; j < chunks[i].startLine; j++) {
      			fgets(wordBuf, LINE_LIMIT, fp);
    		}

    		while((fscanf(fp, "%s", wordBuf)) != EOF) {
      			if (strcmp(wordBuf, "") != 0) {
        			toLower(wordBuf);
        			del(wordBuf);
        			if (*maps == NULL) {
          				*maps = (occurrence *) malloc(sizeof(occurrence));
          				strcpy((*maps)->word.word, wordBuf);
          				(*maps)->word.occurrences = 1;
          				(*maps)->next = NULL;
        			} else {
          				occurrence *mapPointer = *maps;
          				while (strcmp(mapPointer->word.word, wordBuf) != 0 && mapPointer->next != NULL) {
            					mapPointer = mapPointer->next;
          				}
          				if (strcmp(mapPointer->word.word, wordBuf) != 0) {
            					mapPointer->next = (occurrence *) malloc(sizeof(occurrence));
            					mapPointer = mapPointer->next;
            					strcpy(mapPointer->word.word, wordBuf);
            					mapPointer->word.occurrences = 1;
            					mapPointer->next = NULL;
          				} else {
            					mapPointer->word.occurrences++;
          				}
        			}
    			}	
  		}
    		fclose(fp);
    		free(fileFullName);
  	} 
}


