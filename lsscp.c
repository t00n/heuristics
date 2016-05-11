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
#include <stdbool.h>

#include <callback.h>

#include "utils.c"

/** Algorithm parameters **/
int seed=1234567;
char *scp_file="";
char *output_file="output.txt";

/** Variables to activate algorithms **/
int ch1=0, ch2=0, ch3=0, ch4=0, bi=0, fi=0, re=0; 

/** Instance static variables **/
int m;            /* number of elements */
int n;            /* number of subsets */
int **element;        /* element[i] elements that are covered by subsets i */
int **subset;        /* subset[i] subsets that cover element i */
int *nsubset;        /* nsubset[i] number of subsets that cover element i */
int *nelement;        /* nelement[i] number of elements that are covered by subset i */
int *cost;        /* cost[i] cost of subset i  */

/** Solution variables **/
int *x;           /* x[i] 0,1 if subset i is selected */
int *y;           /* y[i] 0,1 if element i covered by the actual solution */
/** Note: Use incremental updates for the solution **/
int fx;           /* sum of the cost of the subsets selected in the solution (can be partial) */ 

/** Dynamic variables **/
/** Note: use dynamic variables to make easier the construction and modification of solutions. **/
/**       these are just examples of useful variables.                                         **/
/**       these variables need to be updated eveytime a subsets is added to a partial solution  **/
/**       or when a complete solution is modified*/
int *subset_cover;   /* subset_subsetver[i] selected subsets that cover element i */
int nsubset_cover;   /* number of selected subsets that cover element i */
int *elements_picked; /* elements already covered by subset_cover */
int nelements_picked; /* number of previous */

void usage(){
    printf("\nUSAGE: lsscp [param_name, param_value] [options]...\n");
    printf("Parameters:\n");
    printf("  --seed : seed to initialize random number generator\n");
    printf("  --instance: SCP instance to execute.\n");
    printf("  --output: Filename for output results.\n");
    printf("Options:\n");
    printf("  --ch1: random solution construction\n");
    printf("  --ch2: static cost-based greedy values.\n");
    printf("  --ch3: static cover cost-based greedy values.\n");
    printf("  --ch4: adapted cover cost-based greedy values\n");
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
    } else if (strcmp(argv[i], "--ch3") == 0) {
      ch3=1;
    } else if (strcmp(argv[i], "--ch4") == 0) {
      ch4=1;
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
  if (fscanf(fp,"%d",&n)!=1)   /* number of subsets */
    error_reading_file("ERROR: there was an error reading instance file.");
  
  /* Cost of the n subsets */
  cost = (int *) mymalloc(n*sizeof(int));
  for (j=0; j<n; j++)
    if (fscanf(fp,"%d",&cost[j]) !=1) 
      error_reading_file("ERROR: there was an error reading instance file."); 
    
  /* Info of subsets that cover each element */
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

int random_pick_element() {
  int elem;
  do {
     elem = rand() % m;
  }
  while (set_member(elem, elements_picked, nelements_picked));
  elements_picked[nelements_picked++] = elem;
  return elem;
}

void add_subset_elems(int subset) {
  for (int i = 0; i < nelement[subset]; ++i) {
    nelements_picked = set_add(element[subset][i], elements_picked, nelements_picked);
  }
}

void construction_search(int (*pick_elem)(), int (*pick_subset)(int)) {
  while (nelements_picked < m) {
    int elem = pick_elem();
    int subset = pick_subset(elem);
    subset_cover[nsubset_cover++] = subset;
    add_subset_elems(subset);
  }
}

int random_pick_subset(int elem) {
  int * available_subsets = subset[elem];
  int navailable_subsets = nsubset[elem];
  int subset;
  do {
    subset = available_subsets[rand() % navailable_subsets];
  }
  while (set_member(subset, subset_cover, nsubset_cover));
  return subset;
}

int greedy_pick_subset(cost_function, alist) 
  float (*cost_function)(int);
  va_alist alist;
{
  va_start_int(alist);
  int elem = va_arg_int(alist);
  int * available_subsets = subset[elem];
  int navailable_subsets = nsubset[elem];
  int subset = available_subsets[0];
  float min_cost = cost_function(subset);
  for (int i = 1; i < navailable_subsets; ++i) {
    float cost = cost_function(available_subsets[i]);
    if (cost < min_cost) {
      min_cost = cost;
      subset = available_subsets[i];
    }
  }
  va_return_int(alist, subset);
}

__TR_function greedy_pick_subset_generator(float (*cost_function)(int)) {
  return alloc_callback(&greedy_pick_subset, cost_function);
}

float static_cost(int subset) {
  return cost[subset];
}

float static_cover_cost(int subset) {
  return (float) cost[subset] / (float)nelement[subset];
}

float adapted_cover_cost(int subset) {
  int count = 0;
  for (int i = 0; i < nelement[subset]; ++i) {
    if (!set_member(element[subset][i], elements_picked, nelements_picked)) {
      count += 1;
    }
  }
  return (float)cost[subset] / (float)count;
}

int find_redundant_sets(int * res) {
  int count = 0;
  for (int i = 0; i < nsubset_cover; ++i) {
    int subset_i = subset_cover[i];
    bool all = true;
    for (int j = 0; j < nelement[subset_i]; ++j) {
      int elem_i = element[subset_i][j];
      bool elem_found = false;
      for (int k = 0; k < nsubset_cover; ++k) {
        if (subset_cover[k] != subset_i) {
          int other_subset = subset_cover[k];
          for (int l = 0; l < nelement[other_subset]; ++l) {
            if (element[other_subset][l] == elem_i) {
              elem_found = true;
              break;
            }
          }
        }
        if (elem_found) {
          break;
        }
      }
      if (! elem_found) {
        all = false;
        break;
      }
    }
    if (all) {
      printf("set %d is redundant\n", subset_i);
      res[count++] = subset_i;
    }
  }
  return count;
}

void eliminate_redundancy(float (*cost_function)(int)) {
  int * redundant_sets = mymalloc(nsubset_cover * sizeof(int));
  int nredundant_sets;
  do {
    nredundant_sets = find_redundant_sets(redundant_sets);
    int max_cost = 0;
    int max_set = -1;
    for (int i = 0; i < nredundant_sets; ++i) {
      int cost = cost_function(redundant_sets[i]);
      if (cost > max_cost) {
        max_cost = cost;
        max_set = redundant_sets[i];
      }
    }
    if (max_set != -1) {
      int res = set_remove(max_set, subset_cover, nsubset_cover);
      if (res != -1) {
        nsubset_cover = res;
      }
    }
  } while (nredundant_sets > 0);
  free(redundant_sets);
}

void compute_solution_variables() {
  for (int i = 0; i < n; ++i) {
    x[i] = 0;
  }
  for (int i = 0; i < nsubset_cover; ++i) {
    x[subset_cover[i]] = 1;
    fx += cost[subset_cover[i]];
  }
  for (int i = 0; i < m; ++i) {
    y[i] = 0;
  }
  for (int i = 0; i < nelements_picked; ++i) {
    y[elements_picked[i]] = 1;
  }
}

// bool is_admissible_solution() {
//   bool * all_elems = mymalloc(m * sizeof(bool));
//   for (int i = 0; i < nsubset_cover; ++i) {
//     int subset_i = subset_cover[i];
//     for (int j = 0; j < nsubset[subset_i]; ++j) {
//       int elem = element[subset_i][j];
//       all_elems[elem] = true;
//     }
//   }
//   bool res = true;
//   for (int i = 0; i < m; ++i) {
//     if (all_elems[i] == false) {
//       res = false;
//       break;
//     }
//   }
//   free(all_elems);
//   return res;
// }

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

void print_solution() {
  // printf("Solution is admissible : %s\n", is_admissible_solution() ? "true" : "false");
  printf("Solution cost : %d\n", fx);
  printf("Solution : %d subsets\n", nsubset_cover);
  for (int i = 0; i < nsubset_cover; ++i) {
    printf("%d, ", subset_cover[i]);
  }
}

/*** Use this function to initialize other variables of the algorithms **/
void initialize(){
  x = mymalloc(n * sizeof(int));
  y = mymalloc(m * sizeof(int));
  fx = 0;
  subset_cover = mymalloc(n * sizeof(int)); // because n is the maximum size if the solution takes all subsets
  nsubset_cover = 0;
  elements_picked = mymalloc(m * sizeof(int));
  nelements_picked = 0;
}
// 
/*** Use this function to finalize execution */
void finalize(){
  free(element);
  free(subset);
  free(nelement);
  free(nsubset);
  free(cost);
  free(elements_picked);
  free(subset_cover);
  free(y);
  free(x);
}

int main(int argc, char *argv[]) {
  read_parameters(argc, argv);
  srand(seed); /*set seed */
  read_scp(scp_file);
  print_instance(1);
  initialize();
  if (ch1) {
    construction_search(random_pick_element, random_pick_subset);
  }
  else if (ch2) {
    __TR_function pick_subset = greedy_pick_subset_generator(static_cost);
    construction_search(random_pick_element, pick_subset);
    free_callback(pick_subset);
  }
  else if (ch3) {
    __TR_function pick_subset = greedy_pick_subset_generator(static_cover_cost);
    construction_search(random_pick_element, pick_subset);
    free_callback(pick_subset);
  }
  else if (ch4) {
    __TR_function pick_subset = greedy_pick_subset_generator(adapted_cover_cost);
    construction_search(random_pick_element, pick_subset);
    free_callback(pick_subset);
  }
  if (re) {
    eliminate_redundancy(static_cost);
  }
  compute_solution_variables();
  print_solution();
  finalize();
  return EXIT_SUCCESS;
}
