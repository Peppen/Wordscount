#include "../include/mpi_file_information.h"
#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// fileNames is an array pointer to FileNames; return value is file number
int getFilesName(file **fileNames, char *dirPath) {
  // Count files number if directory exists
  DIR *dir = opendir(dirPath);
  if (dir == NULL) {
    return 0;
  }
  struct dirent *entry;
  int fileNumber = 0;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type != DT_DIR) {  // Only for files
      fileNumber++;
    }
  }
  closedir(dir);

  // If there is at least one file, get their name
  if (fileNumber == 0) {
    return 0;
  }
  dir = opendir(dirPath);
  if (dir == NULL) {
    return 0;
  }
  *fileNames = (file *)calloc(fileNumber, sizeof(file));

  file *fileNamesPtr = *fileNames;
  while ((entry = readdir(dir)) != NULL) {
    if (entry->d_type != DT_DIR) {
      strcpy(fileNamesPtr->fileName, entry->d_name);
      fileNamesPtr->lineNumber = 0;
      fileNamesPtr++;
    }
  }
  closedir(dir);
  return fileNumber;
}

unsigned long int getLinesNumber(file *fileNames, int fileNumber, char *dirPath) {
  // Read all files using the fileNames
  file *fileNamesPtr = fileNames;
  unsigned long int totalLineNumber = 0;
  for (int i = 0; i < fileNumber; i++) {
    char *fileFullName = (char *)calloc(strlen(dirPath) + strlen(fileNamesPtr->fileName) + 1, sizeof(char));
    strcpy(fileFullName, dirPath);
    strcat(fileFullName, fileNamesPtr->fileName);
    FILE *fp = fopen(fileFullName, "r");
    if (fp == NULL) {
      printf("Error in opening file %s\n", fileNamesPtr->fileName);
    } else {
      char *buf = (char *)malloc(LINE_LIMIT);
      while (fgets(buf, LINE_LIMIT, fp) != NULL) {
        fileNamesPtr->lineNumber++;
        totalLineNumber++;
      }
      fclose(fp);
    }
    fileNamesPtr++;
  }
  return totalLineNumber;
}
