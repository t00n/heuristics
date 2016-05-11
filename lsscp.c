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

int random_pick_element(int * y) {
  int elem;
  do {
     elem = rand() % m;
  }
  while (y[elem]);
  return elem;
}

int add_elem(int elem, int * y) {
  if (!y[elem]) {
    y[elem] = true;
    return 1;
  }
  return 0;
}

bool elem_is_redundant(int elem, int subset, int * x) {
  for (int i = 0; i < n; ++i) {
    if (i != subset && x[i]) {
      for (int j = 0; j < nelement[i]; ++j) {
        if (element[i][j] == elem) {
          return true;        }
      }
    }
  }
  return false;
}

bool subset_is_redundant(int subset, int * x) {
  for (int i = 0; i < nelement[subset]; ++i) {
    int elem = element[subset][i];
    if (!elem_is_redundant(elem, subset, x)) {
      return false;
    }
  }
  return true;
}

int remove_elem(int elem, int * y) {
  if (y[elem]) {
    y[elem] = false;
    return 1;
  }
  return 0;
}

int add_subset_elems(int subset, int * y) {
  int total = 0;
  for (int i = 0; i < nelement[subset]; ++i) {
    if (add_elem(element[subset][i], y)) {
      total++;
    }
  }
  return total;
}

int remove_subset_elems(int subset, int * x, int * y) {
  int total = 0;
  for (int i = 0; i < nelement[subset]; ++i) {
    int elem = element[subset][i];
    if (!elem_is_redundant(elem, subset, x) && remove_elem(elem, y)) {
      total++;
    }
  }
  return total;
}

int add_subset(int subset, int * x, int * y) {
  if (!x[subset]) {
    x[subset] = true;
    fx += cost[subset];
    return add_subset_elems(subset, y);
  }
  return 0;
}

int remove_subset(int subset, int * x, int * y) {
  if (x[subset]) {
    x[subset] = false;
    fx -= cost[subset];
    return remove_subset_elems(subset, x, y);
  }
  return 0;
}

int compute_cost(int * x) {
  int total = 0;
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      total += cost[i];
    }
  }
  return total;
}

int compute_subsets(int * x) {
  int total = 0;
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      total++;
    }
  }
  return total;
}

int compute_elems(int * y) {
  int total = 0;
  for (int i = 0; i < m; ++i) {
    if (y[i]) {
      total++;
    }
  }
  return total;
}

void construction_search(int (*pick_elem)(), int (*pick_subset)(int, int*), int * x, int * y) {
  int nelements_picked = compute_elems(y);
  while (nelements_picked < m) {
    int elem = pick_elem(y);
    nelements_picked += add_elem(elem, y);
    int subset = pick_subset(elem, x);
    nelements_picked += add_subset(subset, x, y);
  }
}

int random_pick_subset(int elem, int * x) {
  int * available_subsets = subset[elem];
  int navailable_subsets = nsubset[elem];
  int subset;
  do {
    subset = available_subsets[rand() % navailable_subsets];
  } while (x[subset]);
  return subset;
}

int greedy_pick_subset(cost_function, alist) 
  float (*cost_function)(int, int*);
  va_alist alist;
{
  va_start_int(alist);
  int elem = va_arg_int(alist);
  int * x = va_arg_ptr(alist, int*);
  int * available_subsets = subset[elem];
  int navailable_subsets = nsubset[elem];
  int subset = available_subsets[0];
  float min_cost = cost_function(subset, x);
  for (int i = 1; i < navailable_subsets; ++i) {
    float cost = cost_function(available_subsets[i], x);
    if (cost < min_cost) {
      min_cost = cost;
      subset = available_subsets[i];
    }
  }
  va_return_int(alist, subset);
  return 0;
}

__TR_function greedy_pick_subset_generator(float (*cost_function)(int, int*)) {
  return alloc_callback(&greedy_pick_subset, cost_function);
}

float static_cost(int subset, int * y) {
  return cost[subset];
}

float static_cover_cost(int subset, int * y) {
  return (float) cost[subset] / (float)nelement[subset];
}

float adapted_cover_cost(int subset, int * y) {
  int count = 0;
  for (int i = 0; i < nelement[subset]; ++i) {
    if (!y[element[subset][i]]) {
      count += 1;
    }
  }
  return (float)cost[subset] / (float)count;
}

int find_redundant_subsets(int * res, int * x) {
  int count = 0;
  for (int i = 0; i < n; ++i) {
    if (x[i] && subset_is_redundant(i, x)) {
      res[count++] = i;
    }
  }
  return count;
}

void eliminate_redundancy(float (*cost_function)(int, int*), int * x, int * y) {
  int * redundant_sets = mymalloc(compute_subsets(x) * sizeof(int));
  int nredundant_sets;
  do {
    nredundant_sets = find_redundant_subsets(redundant_sets, x);
    int max_cost = 0;
    int max_set = -1;
    for (int i = 0; i < nredundant_sets; ++i) {
      int cost = cost_function(redundant_sets[i], y);
      if (cost > max_cost) {
        max_cost = cost;
        max_set = redundant_sets[i];
      }
    }
    if (max_set != -1) {
      remove_subset(max_set, x, y);
    }
  } while (nredundant_sets > 0);
  free(redundant_sets);
}

float first_improvement(int * work_subsets, int * work_elems, int * x, int * y, int current_cost) {
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      memcpy(work_subsets, x, n * sizeof(int));
      memcpy(work_elems, y, m * sizeof(int));
      remove_subset(i, work_subsets, work_elems);
      __TR_function pick_subset = greedy_pick_subset_generator(static_cost);
      construction_search(random_pick_element, pick_subset, work_subsets, work_elems);
      free_callback(pick_subset);
      int cost = compute_cost(work_subsets);
      if (cost < current_cost) {
        memcpy(x, work_subsets, n * sizeof(int));
        memcpy(y, work_elems, m * sizeof(int));
        return cost;
      }
    }
  }
  return INFINITY;
}

void iterative_search(float (*improvement_function)(int*, int*, int*, int*, int), int * x, int * y) {
  float current_cost = compute_cost(x);
  bool improvement;
  int * work_subsets = mymalloc(n * sizeof(int));
  int * work_elems = mymalloc(m * sizeof(int));
  do {
    improvement = false;
    float cost = improvement_function(work_subsets, work_elems, x, y, current_cost);
    if (cost < current_cost) {
      improvement = true;
      current_cost = cost;
    }
    eliminate_redundancy(static_cost, x, y);
  } while (improvement);
  free(work_elems);
  free(work_subsets);
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
  printf("Solution cost : %d\n", compute_cost(x));
  printf("Solution : %d subsets\n", compute_subsets(x));
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      printf("%d, ", i);
    }
  }
}

/*** Use this function to initialize other variables of the algorithms **/
void initialize(){
  x = mymalloc(n * sizeof(int));
  for (int i = 0; i < n; ++i) {
    x[i] = false;
  }
  y = mymalloc(m * sizeof(int));
  for (int i = 0; i < m; ++i) {
    y[i] = false;
  }
  fx = 0;
}
// 
/*** Use this function to finalize execution */
void finalize(){
  free(element);
  free(subset);
  free(nelement);
  free(nsubset);
  free(cost);
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
    construction_search(random_pick_element, random_pick_subset, x, y);
  }
  else if (ch2) {
    __TR_function pick_subset = greedy_pick_subset_generator(static_cost);
    construction_search(random_pick_element, pick_subset, x, y);
    free_callback(pick_subset);
  }
  else if (ch3) {
    __TR_function pick_subset = greedy_pick_subset_generator(static_cover_cost);
    construction_search(random_pick_element, pick_subset, x, y);
    free_callback(pick_subset);
  }
  else if (ch4) {
    __TR_function pick_subset = greedy_pick_subset_generator(adapted_cover_cost);
    construction_search(random_pick_element, pick_subset, x, y);
    free_callback(pick_subset);
  }
  if (re) {
    eliminate_redundancy(static_cost, x, y);
  }
  if (fi) {
    iterative_search(first_improvement, x, y);
  }
  else if (bi) {
    // iterative_search(x, y);
  }
  // compute_solution_variables();
  print_solution();
  finalize();
  return EXIT_SUCCESS;
}
