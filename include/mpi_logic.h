#include "mpi_helper.h"

typedef struct Word {
  char word[LINE_LIMIT];
  unsigned long int occurrences;
} word;

typedef struct Occurrence {
  word word;
  struct Occurrence *next;
} occurrence;

void count_words(occurrence **maps, chunk *myChunks, int myChunkNumber, char *dirPath);
int getWordsNumber(occurrence *words);
void createWordDatatype(MPI_Datatype *wordData);
void toLower(char *s);
void del(char *line);
void change(word **maps, occurrence *occurrences);
