
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

bool set_member(int elem, int* array, int narray) {
	for (int i = 0; i < narray; i++) {
		if (array[i] == elem) {
			return true;
		}
	}
	return false;
}

int set_add(int elem, int* array, int narray) {
	if (! set_member(elem, array, narray)) {
	  array[narray++] = elem;
	}
	return narray;
}


int set_remove(int elem, int* array, int narray) {
    int i = 0;
    while (array[i] != elem && i < narray) ++i;
    if (i < narray) {
        for (int j = i; j < narray-1; ++j) {
            array[j] = array[j+1];
        }
        return narray - 1;
    }
    return -1;
}