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
int ch1=0, ch2=0, ch3=0, ch4=0, bi=0, fi=0, re=0, ig=0, dls=0, gen=0, iter=100; 

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
__TR_function static_cost_greedy, static_cover_cost_greedy, adapted_cover_cost_greedy;

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
    printf("  --ig: iterated greedy\n");
    printf("  --dls: dynamic local search\n");
    printf("  --gen: genetic algorithm\n");
    printf("  --iter: number of iterations for SLS methods\n");
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
    } else if (strcmp(argv[i], "--ig") == 0) {
      ig=1;
    } else if (strcmp(argv[i], "--dls") == 0) {
      dls=1;
    } else if (strcmp(argv[i], "--gen") == 0) {
      gen=1;
    } else if (strcmp(argv[i], "--iter") == 0) {
      iter=atoi(argv[i+1]);
      i+=1;
    } else {
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

// pick a non covered element at random
int random_pick_element(int * y) {
  int elem;
  do {
     elem = rand() % m;
  }
  while (y[elem]);
  return elem;
}

// add elem to the solution if non covered
int add_elem(int elem, int * y) {
  if (!y[elem]) {
    y[elem] = true;
    return 1;
  }
  return 0;
}

// check in the solution x if the elem in subset is redundant
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

// check in the solution x if the subset is redundant
bool subset_is_redundant(int subset, int * x) {
  for (int i = 0; i < nelement[subset]; ++i) {
    int elem = element[subset][i];
    if (!elem_is_redundant(elem, subset, x)) {
      return false;
    }
  }
  return true;
}

// remove elem from the solution if covered
int remove_elem(int elem, int * y) {
  if (y[elem]) {
    y[elem] = false;
    return 1;
  }
  return 0;
}

// add every non covered elem of subset to the solution
int add_subset_elems(int subset, int * y) {
  int total = 0;
  for (int i = 0; i < nelement[subset]; ++i) {
    if (add_elem(element[subset][i], y)) {
      total++;
    }
  }
  return total;
}

// remove every covered elem of subset from the solution
int remove_subset_elems(int subset, int * x, int * y) {
  int total = 0;
  for (int i = 0; i < nelement[subset]; ++i) {
    int elem = element[subset][i];
    // remove an elem only if it is not redundant
    if (!elem_is_redundant(elem, subset, x) && remove_elem(elem, y)) {
      total++;
    }
  }
  return total;
}

// add subset and its elements to the solution
int add_subset(int subset, int * x, int * y) {
  if (!x[subset]) {
    x[subset] = true;
    fx += cost[subset];
    return add_subset_elems(subset, y);
  }
  return 0;
}

// remove subset and its elements from the solution
int remove_subset(int subset, int * x, int * y) {
  if (x[subset]) {
    x[subset] = false;
    fx -= cost[subset];
    return remove_subset_elems(subset, x, y);
  }
  return 0;
}

// compute the raw cost of the solution x
int compute_cost(int * x) {
  int total = 0;
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      total += cost[i];
    }
  }
  return total;
}

int size_subsets(int * x) {
  int total = 0;
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      total++;
    }
  }
  return total;
}

int size_elems(int * y) {
  int total = 0;
  for (int i = 0; i < m; ++i) {
    if (y[i]) {
      total++;
    }
  }
  return total;
}

// generic construction search. use the function pointers pick_elem and pick_subset to respectively pick a non covered element and a subset that covers this element
void construction_search(int (*pick_elem)(), int (*pick_subset)(int, int*), int * x, int * y) {
  int nelements_picked = size_elems(y);
  while (nelements_picked < m) {
    int elem = pick_elem(y);
    nelements_picked += add_elem(elem, y);
    int subset = pick_subset(elem, x);
    nelements_picked += add_subset(subset, x, y);
  }
}

// pick a subset that covers elem at random
int random_pick_subset(int elem, int * x) {
  int * available_subsets = subset[elem];
  int navailable_subsets = nsubset[elem];
  int subset;
  do {
    subset = available_subsets[rand() % navailable_subsets];
  } while (x[subset]);
  return subset;
}

// pick the subset that covers elem with the best cost using the cost_function pointer
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

// static cost of subset
float static_cost(int subset, int * y) {
  return cost[subset];
}

// static cost divided by static cover
float static_cover_cost(int subset, int * y) {
  return (float) cost[subset] / (float)nelement[subset];
}

// static cost divided by adapted cover
float adapted_cover_cost(int subset, int * y) {
  int count = 0;
  for (int i = 0; i < nelement[subset]; ++i) {
    if (!y[element[subset][i]]) {
      count += 1;
    }
  }
  return (float)cost[subset] / (float)count;
}

// put redundant subsets of x in res
int find_redundant_subsets(int * res, int * x) {
  int count = 0;
  for (int i = 0; i < n; ++i) {
    if (x[i] && subset_is_redundant(i, x)) {
      res[count++] = i;
    }
  }
  return count;
}

// remove redundant sets from x, beginning with highest cost (use cost_function function pointer)
void eliminate_redundancy(int * x, int * y) {
  int * redundant_sets = mymalloc(size_subsets(x) * sizeof(int));
  int nredundant_sets;
  do {
    nredundant_sets = find_redundant_subsets(redundant_sets, x);
    int max_cost = 0;
    int max_set = -1;
    for (int i = 0; i < nredundant_sets; ++i) {
      int current_cost = cost[redundant_sets[i]];
      if (current_cost > max_cost) {
        max_cost = current_cost;
        max_set = redundant_sets[i];
      }
    }
    if (max_set != -1) {
      remove_subset(max_set, x, y);
    }
  } while (nredundant_sets > 0);
  free(redundant_sets);
}

// this functions finds a neighbour that improves the cost. It is parametrized by the type : "first" or "best" to find the first or best improvement respectively
int find_improvement(char * type, int * work_subsets, int * work_elems, int * x, int * y, int (*cost_function)(int*)) {
  int current_cost = cost_function(work_subsets);
  // remove each subset from the solution one at a time and construct a new solution. This gives a neighbour
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      memcpy(work_subsets, x, n * sizeof(int));
      memcpy(work_elems, y, m * sizeof(int));
      remove_subset(i, work_subsets, work_elems);
      construction_search(random_pick_element, static_cover_cost_greedy, work_subsets, work_elems);
      int cost = cost_function(work_subsets);
      if (cost < current_cost) {
        memcpy(x, work_subsets, n * sizeof(int));
        memcpy(y, work_elems, m * sizeof(int));
        if (strcmp(type, "first") == 0) {
          return cost;
        }
      }
    }
  }
  return current_cost;
}

// performs a perturbative search to find a local minimum
void perturbative_search(char * type, int * x, int * y, int (*cost_function)(int*)) {
  int current_cost = cost_function(x);
  bool improvement;
  int * work_subsets = mymalloc(n * sizeof(int));
  int * work_elems = mymalloc(m * sizeof(int));
  do {
    improvement = false;
    int cost = find_improvement(type, work_subsets, work_elems, x, y, cost_function);
    if (cost < current_cost) {
      improvement = true;
      current_cost = cost;
    }
    eliminate_redundancy(x, y);
  } while (improvement);
  free(work_elems);
  free(work_subsets);
}

// remove one element at random and then complete the solution at random
void reconstruct2(int * x, int * y) {
  int subset_to_remove;
  do {
    subset_to_remove = rand() % n;
  } while (!x[subset_to_remove]);
  remove_subset(subset_to_remove, x, y);
  construction_search(random_pick_element, adapted_cover_cost_greedy, x, y);
}

// apply n random move then remove redundant sets
void reconstruct(int * x, int * y, int n) {
  for (int i = 0; i < n; ++i) {
    reconstruct2(x, y);
  }
  eliminate_redundancy(x, y);
}

void iterated_greedy(int * x, int * y, int steps) {
  // generate inital solution
  construction_search(random_pick_element, adapted_cover_cost_greedy, x, y);
  // perform a local search
  perturbative_search("best", x, y, compute_cost);
  // work variables
  int * work_subsets = mymalloc(n * sizeof(int));
  int * work_elems = mymalloc(m * sizeof(int));
  int min_cost = compute_cost(x);
  for (int i = 0; i < steps; ++i) {
    memcpy(work_subsets, x, n * sizeof(int));
    memcpy(work_elems, y, m * sizeof(int));
    // random 2-move
    reconstruct(work_subsets, work_elems, 1);
    // local search
    perturbative_search("best", work_subsets, work_elems, compute_cost);
    int cost = compute_cost(work_subsets);
    // save new solution if better
    if (cost < min_cost) {
      memcpy(x, work_subsets, n * sizeof(int));
      memcpy(y, work_elems, m * sizeof(int));
      min_cost = cost;
    }
  }
  free(work_elems);
  free(work_subsets);
}

int dynamic_cost(penalties, alist) 
  int * penalties;
  va_alist alist;
{
  va_start_int(alist);
  int * x = va_arg_ptr(alist, int *);
  int res = 0;
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      res += penalties[i] * cost[i];
    }
  }
  va_return_int(alist, res);
  return 0;
}

// [Voudouris and Tsang, 1995] 
void update_penalties(int * x, int * penalties) {
  int total_cost = compute_cost(x);
  float utility(int i) {
    return ((float)total_cost/(float)cost[i]) / (1 + (float)penalties[i]);
  }
  float max_utility = 0;
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      float util = utility(i);
      if (util > max_utility) {
        max_utility = util;
      }
    }
  }
  for (int i = 0; i < n; ++i) {
    if (x[i] && utility(i) >= max_utility) {
      penalties[i] += 1;
    }
  }
}

void dynamic_local_search(int * x, int * y, int steps) {
  construction_search(random_pick_element, adapted_cover_cost_greedy, x, y);
  int * penalties = mymalloc(n * sizeof(int));
  __TR_function cost_function = alloc_callback(&dynamic_cost, penalties);
  for (int i = 0; i < n; ++i) {
    penalties[i] = 0;
  }
  do {
    perturbative_search("best", x, y, cost_function);
    update_penalties(x, penalties);
    steps--;
  } while (steps > 0);
  free_callback(cost_function);
}

int pick_population(int * picked, int T, int * population_cost, int population_size) {
  int * pool = mymalloc(T * sizeof(int));
  for (int i = 0; i < T; ++i) {
    int pick;
    do {
      pick = rand() % population_size;
    } while (picked[pick]);
    pool[i] = pick;
  }
  int pick = pool[0];
  int min_cost = population_cost[pick];
  for (int i = 1; i < T; ++i) {
    if (population_cost[pool[i]] < min_cost) {
      min_cost = population_cost[pool[i]];
      pick = pool[i];
    }
  }
  free(pool);
  return pick;
}

void genetic_algorithm(int * x, int * y, int steps) {
  int population_size = 100;
  int T = 2;
  // Generate initial population using random construction search
  int ** population = mymalloc(population_size * sizeof(int*));
  int * dummy_elements = mymalloc(m * sizeof(int));
  for (int i = 0; i < population_size; ++i) {
    population[i] = mymalloc(n * sizeof(int));
    for (int j = 0; j < m; ++j) {
      dummy_elements[j] = 0;
    }
    construction_search(random_pick_element, random_pick_subset, population[i], dummy_elements);
    perturbative_search("first", population[i], dummy_elements, compute_cost);
    // print_solution(population[i], dummy_elements);
    // printf("meme pas ici\n");
  }
  // Compute costs for the initial population
  int * population_cost = mymalloc(population_size * sizeof(int));
  for (int i = 0; i < population_size; ++i) {
    population_cost[i] = compute_cost(population[i]);
  }
  int * picked = mymalloc(population_size * sizeof(int));
  int * children = mymalloc(n * sizeof(int));
  while (steps > 0) {
    for (int i = 0; i < population_size; ++i) {
      picked[i] = 0;
    }
    // Select parents
    int first_parent = pick_population(picked, T, population_cost, population_size);
    int second_parent = pick_population(picked, T, population_cost, population_size);
    // recombine parents -> children
    // evaluate children
    // replace some parents by children
    steps--;
  }
  free(children);
  free(picked);
  // Find best solution of population
  int best_population = 0;
  int min_cost = population_cost[best_population];
  for (int i = 1; i < population_size; ++i) {
    if (population_cost[i] < min_cost) {
      min_cost = population_cost[i];
      best_population = i;
    }
  }
  // Copy best solution and free memory
  memcpy(x, population[best_population], n * sizeof(int));
  free(population_cost);
  for (int i = 0; i < 100; ++i) {
    free(population[i]);
  }
  free(dummy_elements);
  free(population);
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

void print_solution(int * x, int * y) {
  // printf("Solution is admissible : %s\n", is_admissible_solution() ? "true" : "false");
  printf("Solution cost : %d\n", compute_cost(x));
  printf("Solution : %d subsets\n", size_subsets(x));
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      printf("%d, ", i);
    }
  }
  // printf("\nSolution : %d elements", size_elems(y));
  // for (int i = 0; i < m; ++i) {
  //   if (y[i]) printf("%d ", i);
  // }
  printf("\n");
}

void print_cost(int * x) {
  printf("%d\n", compute_cost(x));
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
  static_cost_greedy = greedy_pick_subset_generator(static_cost);
  static_cover_cost_greedy = greedy_pick_subset_generator(static_cover_cost);
  adapted_cover_cost_greedy = greedy_pick_subset_generator(adapted_cover_cost);
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
  free_callback(adapted_cover_cost_greedy);
  free_callback(static_cover_cost_greedy);
  free_callback(static_cost_greedy);
}

int main(int argc, char *argv[]) {
  read_parameters(argc, argv);
  srand(seed); /*set seed */
  read_scp(scp_file);
  // print_instance(0);
  initialize();
  // The four construction searches. The only difference is the cost computation technique
  if (ch1) {
    construction_search(random_pick_element, random_pick_subset, x, y);
  }
  else if (ch2) {
    construction_search(random_pick_element, static_cost_greedy, x, y);
  }
  else if (ch3) {
    construction_search(random_pick_element, static_cover_cost_greedy, x, y);
  }
  else if (ch4) {
    construction_search(random_pick_element, adapted_cover_cost_greedy, x, y);
  }
  if (re) {
    eliminate_redundancy(x, y);
  }
  if (fi) {
    perturbative_search("first", x, y, compute_cost);
  }
  else if (bi) {
    perturbative_search("best", x, y, compute_cost);
  }
  else if (ig) {
    iterated_greedy(x, y, iter);
  }
  else if (dls) {
    dynamic_local_search(x, y, iter);
  }
  else if (gen) {
    genetic_algorithm(x, y, iter);
  }
  // compute_solution_variables();
  // print_solution(x, y);
  print_cost(x);
  finalize();
  return EXIT_SUCCESS;
}
