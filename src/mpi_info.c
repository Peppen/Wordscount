#include "../include/mpi_info.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void time_info(int p, int fileNumber, int totalLines, double time) {
      printf("Writing on file %s\n", TIME_FILE);
      FILE *f = fopen(TIME_FILE, "a");
      if(f != NULL) {
         fseek (f, 0, SEEK_END);
         int size = ftell(f);

         if (size == 0) 
           fprintf(f, "---Time Log File---\n"); 
           
         fprintf(f, "---%d Process(es) working on %d file(s) containing %d lines---\n", p, fileNumber, totalLines);
         fprintf(f, "Average Global Time for Wordcounting -> %f seconds\n", time);
      }
      
      fclose(f);
}
