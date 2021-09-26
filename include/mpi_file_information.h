#define FILE_NAME_SIZE 256
#define LINE_LIMIT 1024

typedef struct FileName {
  char fileName[FILE_NAME_SIZE];
  unsigned long int lineNumber;
} file;

int getFilesName(file **fileNames, char *dirPath);
unsigned long int getLinesNumber(file *fileNames, int fileNumber, char *dirPath);
void getNumberWords(file *fileNames, int fileNumber, char *dirPath);
