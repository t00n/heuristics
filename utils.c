
#include <stdlib.h>
#include <limits.h>
#include <stdbool.h>

void *mymalloc( size_t size ) {
  void *s;
  if ( (s=malloc(size)) == NULL ) {
    fprintf( stderr, "malloc : Not enough memory.\n" );
    exit( EXIT_FAILURE );
  }
  return s;
}

void error_reading_file(char *text){
  printf("%s\n", text);
  exit( EXIT_FAILURE );
}

bool is_in_array(int elem, int* array, int narray) {
    for (int i = 0; i < narray; i++) {
        if (array[i] == elem) {
            return true;
        }
    }
    return false;
}
