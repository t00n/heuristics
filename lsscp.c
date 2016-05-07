/**      Heuristic Optimization     **/
/**              2016               **/    
/**       Template exercise 1       **/
/**       Set Covering Problem      **/
/**                                 **/
/*************************************/
/** For this code:                  **/
/**   elements = elements               **/
/**   subsets = subsets                **/
/*************************************/

/** Code implemented for Heuritics optimization class by:  **/
/** Antoine CARPENTIER                                     **/

/** Note: Remember to keep your code in order and properly commented. **/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <string.h>

#include "utils.c"

/** Algorithm parameters **/
int seed=1234567;
char *scp_file="";
char *output_file="output.txt";

/** Variables to activate algorithms **/
int ch1=0, ch2=0, bi=0, fi=0, re=0; 

/** Instance static variables **/
int m;            /* number of elements */
int n;            /* number of subsetss */
int **element;        /* element[i] elements that are covered by subsets i */
int **subset;        /* subset[i] subsetss that cover element i */
int *nsubset;        /* nsubset[i] number of subsetss that cover element i */
int *nelement;        /* nelement[i] number of elements that are covered by subsets i */
int *cost;        /* cost[i] cost of subsets i  */

/** Solution variables **/
int *x;           /* x[i] 0,1 if subsets i is selected */
int *y;           /* y[i] 0,1 if element i covered by the actual solution */
/** Note: Use incremental updates for the solution **/
int fx;           /* sum of the cost of the subsetss selected in the solution (can be partial) */ 

/** Dinamic variables **/
/** Note: use dinamic variables to make easier the construction and modification of solutions. **/
/**       these are just examples of useful variables.                                         **/
/**       these variables need to be updated eveytime a subsets is added to a partial solution  **/
/**       or when a complete solution is modified*/
int *subset_cover;   /* subset_subsetver[i] selected subsetss that cover element i */
int nsubset_cover;   /* number of selected subsetss that cover element i */

void usage(){
    printf("\nUSAGE: lsscp [param_name, param_value] [options]...\n");
    printf("Parameters:\n");
    printf("  --seed : seed to initialize random number generator\n");
    printf("  --instance: SCP instance to execute.\n");
    printf("  --output: Filename for output results.\n");
    printf("Options:\n");
    printf("  --ch1: random solution construction\n");
    printf("  --ch2: static cost-based greedy values.\n");
    printf("  --re: applies redundancy elimination after construction.\n");
    printf("  --bi: best improvement.\n");
    printf("\n");
}



/*Read parameters from command line*/
/*NOTE: Complete parameter list*/
void read_parameters(int argc, char *argv[]) {
  int i;
  if(argc<=1) {
    usage();
    exit(EXIT_FAILURE);
  }
  for(i=1;i<argc;i++){
    if (strcmp(argv[i], "--seed") == 0) {
      seed=atoi(argv[i+1]);
      i+=1;
    } else if (strcmp(argv[i], "--instance") == 0) {
      scp_file=argv[i+1];
      i+=1;
    } else if (strcmp(argv[i], "--output") == 0) {
      output_file=argv[i+1];
      i+=1;            
    } else if (strcmp(argv[i], "--ch1") == 0) {
      ch1=1;
    } else if (strcmp(argv[i], "--ch2") == 0) {
      ch2=1;
    } else if (strcmp(argv[i], "--bi") == 0) {
      bi=1;
    } else if (strcmp(argv[i], "--fi") == 0) {
      fi=1;
    } else if (strcmp(argv[i], "--re") == 0) {
      re=1;
    }else{
      printf("\nERROR: parameter %s not recognized.\n",argv[i]);
      usage();
      exit( EXIT_FAILURE );
    }
  }
  
  if( (scp_file == NULL) || ((scp_file != NULL) && (scp_file[0] == '\0'))){
    printf("Error: --instance must be provided.\n");
    usage();
    exit( EXIT_FAILURE );
  }
  
}

/*** Read instance in the OR-LIBRARY format ***/
void read_scp(char *filename) {
  int h,i,j;
  int *k;
  FILE *fp = fopen(filename, "r" );
  
  if (fscanf(fp,"%d",&m)!=1)   /* number of elements */
    error_reading_file("ERROR: there was an error reading instance file.");
  if (fscanf(fp,"%d",&n)!=1)   /* number of subsetss */
    error_reading_file("ERROR: there was an error reading instance file.");
  
  /* Cost of the n subsetss */
  cost = (int *) mymalloc(n*sizeof(int));
  for (j=0; j<n; j++)
    if (fscanf(fp,"%d",&cost[j]) !=1) 
      error_reading_file("ERROR: there was an error reading instance file."); 
    
  /* Info of subsetss that cover each element */
  subset  = (int **) mymalloc(m*sizeof(int *));
  nsubset = (int *) mymalloc(m*sizeof(int));
  for (i=0; i<m; i++) {
    if (fscanf(fp,"%d",&nsubset[i])!=1)
      error_reading_file("ERROR: there was an error reading instance file.");
    subset[i] = (int *) mymalloc(nsubset[i]*sizeof(int));
    for (h=0; h<nsubset[i]; h++) {
      if( fscanf(fp,"%d",&subset[i][h])!=1 )
        error_reading_file("ERROR: there was an error reading instance file."); 
      subset[i][h]--;
    }
  }
  
  /* Info of elements that are covered by each subsets */
  element  = (int **) mymalloc(n*sizeof(int *));
  nelement = (int *) mymalloc(n*sizeof(int));
  k    = (int *) mymalloc(n*sizeof(int));
  for (j=0; j<n; j++) nelement[j]=0;
  for (i=0; i<m; i++) {
    for (h=0; h<nsubset[i]; h++)
      nelement[subset[i][h]]++;
  }
  for (j=0; j<n; j++) {
    element[j] = (int *) mymalloc(nelement[j]*sizeof(int));
    k[j]   = 0;
  }
  for (i=0;i<m;i++) {
    for (h=0;h<nsubset[i];h++) {
      element[subset[i][h]][k[subset[i][h]]] = i;
      k[subset[i][h]]++;
    }
  }
  free((void *)k);
}

/*** Use level>=1 to print more info (check the correct reading) */ 
void print_instance(int level){
  int i;
  
  printf("**********************************************\n");
  printf("  SCP INSTANCE: %s\n", scp_file);
  printf("  PROBLEM SIZE\t m = %d\t n = %d\n", m, n);
  
  if(level >=1){
    printf("  SUBSET COST:\n");
    for(i=0; i<n;i++)
      printf("%d ",cost[i]);
    printf("\n\n");
    printf("  NUMBER OF ELEMENTS COVERED BY SUBSET 1 is %d\n", nelement[0] );
    for(i=0; i<nelement[0];i++)
      printf("%d ", element[0][i]);
    printf("\n");
    printf("  NUMBER OF SUBSETS COVERING ELEMENT 1 is %d\n", nsubset[0] );
    for(i=0; i<nsubset[0];i++)
      printf("%d ", subset[0][i]);
    printf("\n");
  }
 
  printf("**********************************************\n\n");

}

/*** Use this function to initialize other variables of the algorithms **/
void initialize(){


}

/*** Use this function to finalize execution */
void finalize(){
  free((void **) element );
  free((void **) subset );
  free((void *) nelement );
  free((void *) nsubset );
  free((void *) cost );
}

int main(int argc, char *argv[]) {
  read_parameters(argc, argv);
  srand(seed); /*set seed */
  read_scp(scp_file);
  print_instance(1);
  finalize();
  return EXIT_SUCCESS;
}
