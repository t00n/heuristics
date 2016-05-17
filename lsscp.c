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
#include <assert.h>

#include <callback.h>

#include "utils.c"

/** Algorithm parameters **/
int seed=1234567;
char *scp_file="";
char *output_file="output.txt";

/** Variables to activate algorithms **/
int ch1=0, ch2=0, ch3=0, ch4=0, bi=0, fi=0, re=0, ig=0, gen=0, t=100; 

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
    printf("  --t: time to run for SLS methods (in ms)\n");
    printf("Options:\n");
    printf("  --ch1: random solution construction\n");
    printf("  --ch2: static cost-based greedy values.\n");
    printf("  --ch3: static cover cost-based greedy values.\n");
    printf("  --ch4: adapted cover cost-based greedy values\n");
    printf("  --re: applies redundancy elimination after construction.\n");
    printf("  --bi: best improvement.\n");
    printf("  --ig: iterated greedy\n");
    printf("  --gen: genetic algorithm\n");
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
    } else if (strcmp(argv[i], "--gen") == 0) {
      gen=1;
    } else if (strcmp(argv[i], "--t") == 0) {
      t=atoi(argv[i+1]);
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

/************* Helper functions to manipulate elements and subsets *************/

// add elem to the solution if non covered
int add_elem(int elem, int * y) {
  if (!y[elem]) {
    y[elem] = true;
    return 1;
  }
  return 0;
}

// remove elem from the solution if covered
int remove_elem(int elem, int * y) {
  if (y[elem]) {
    y[elem] = false;
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
    return add_subset_elems(subset, y);
  }
  return 0;
}

// remove subset and its elements from the solution
int remove_subset(int subset, int * x, int * y) {
  if (x[subset]) {
    x[subset] = false;
    return remove_subset_elems(subset, x, y);
  }
  return 0;
}

// compute the raw cost of the solution x
int total_cost_raw(int * x) {
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

/************* Construction search (CH1 -> CH4) *************/

// generic construction search. use the function pointers pick_elem and pick_subset to respectively pick a non covered element and a subset that covers this element. the cost_function pointer is initialized to the function to compute the cost of a single subset
void construction_search(int (*pick_elem)(), int (*pick_subset)(float (*)(int, int*), int, int*, int*), int * x, int * y, float (*cost_function)(int, int*)) {
  while (size_elems(y) < m) {
    int elem = pick_elem(y);
    int subset = pick_subset(cost_function, elem, x, y);
    add_subset(subset, x, y);
  }
}

// pick a non covered element at random
int random_pick_element(int * y) {
  for (int i = 0; i < m; ++i) {
    if (!y[i]) return i;
  }
}

// pick a non used subset that covers elem at random
int random_pick_subset(float (*cost_function)(int, int*), int elem, int * x, int *y) {
  int * available_subsets = subset[elem];
  int navailable_subsets = nsubset[elem];
  int subset;
  do {
    subset = available_subsets[rand() % navailable_subsets];
  } while (x[subset]);
  return subset;
}

// pick the subset that covers elem with the best cost using the cost_function pointer
int greedy_pick_subset(float (*cost_function)(int, int*), int elem, int * x, int * y)
{
  int * available_subsets = subset[elem];
  int navailable_subsets = nsubset[elem];
  int subset = available_subsets[0];
  float min_cost = cost_function(subset, y);
  for (int i = 1; i < navailable_subsets; ++i) {
    float cost = cost_function(available_subsets[i], y);
    if (cost < min_cost) {
      min_cost = cost;
      subset = available_subsets[i];
    }
  }
  return subset;
}

// static cost of subset. passed to greedy_pick_subset
float static_cost(int subset, int * y) {
  return cost[subset];
}

// static cost divided by static cover. passed to greedy_pick_subset
float static_cover_cost(int subset, int * y) {
  return (float) cost[subset] / (float)nelement[subset];
}

// static cost divided by adapted cover. passed to greedy_pick_subset
float adapted_cover_cost(int subset, int * y) {
  int count = 0;
  for (int i = 0; i < nelement[subset]; ++i) {
    if (!y[element[subset][i]]) {
      count += 1;
    }
  }
  return (float)cost[subset] / (float)count;
}

/************* Redundancy elimination (RE) *************/

// put redundant subsets of the solution x in res
int find_redundant_subsets(int * res, int * x) {
  int count = 0;
  for (int i = 0; i < n; ++i) {
    if (x[i] && subset_is_redundant(i, x)) {
      res[count++] = i;
    }
  }
  return count;
}

// remove redundant sets from x, beginning with highest cost
void eliminate_redundancy(int * x, int * y) {
  int * redundant_sets = mymalloc(size_subsets(x) * sizeof(int));
  int nredundant_sets;
  do {
    nredundant_sets = find_redundant_subsets(redundant_sets, x);
    int max_cost = 0;
    int max_set = -1;
    for (int i = 0; i < nredundant_sets; ++i) {
      int current_cost = static_cost(redundant_sets[i], y);
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

/************* Perturbative search (FI, BI) *************/

// this functions finds a neighbour that improves the cost. It is parametrized by the type : "first" or "best" to find the first or best improvement respectively
int find_improvement(char * type, int * work_subsets, int * work_elems, int * x, int * y, int (*total_cost_function)(int*)) {
  int current_cost = total_cost_function(work_subsets);
  // remove each subset from the solution one at a time and construct a new solution. This gives a neighbour
  for (int i = 0; i < n; ++i) {
    if (x[i]) {
      memcpy(work_subsets, x, n * sizeof(int));
      memcpy(work_elems, y, m * sizeof(int));
      remove_subset(i, work_subsets, work_elems);
      construction_search(random_pick_element, greedy_pick_subset, work_subsets, work_elems, static_cost);
      int cost = total_cost_function(work_subsets);
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

/************* Hybrid method *************/

void iterated_greedy(int * x, int * y, int t, float alpha) {
  // generate inital solution
  construction_search(random_pick_element, greedy_pick_subset, x, y, adapted_cover_cost);
  // perform a local search
  perturbative_search("best", x, y, total_cost_raw);
  // work variables
  int * work_subsets = mymalloc(n * sizeof(int));
  int * work_elems = mymalloc(m * sizeof(int));
  int min_cost = total_cost_raw(x);
  float start = clock()/CLOCKS_PER_SEC, stop = (float)t/1000.0;
  do {
    for (int i = 0; i < n; ++i) {
      work_subsets[i] = 0;
    }
    for (int i = 0; i < m; ++i) {
      work_elems[i] = 0;
    }
    construction_search(random_pick_element, greedy_pick_subset, work_subsets, work_elems, adapted_cover_cost);
    perturbative_search("best", work_subsets, work_elems, total_cost_raw);
    int cost = total_cost_raw(work_subsets);
    // save new solution if better
    if (cost < min_cost) {
      swap_ptr(x, work_subsets);
      swap_ptr(y, work_elems);
      min_cost = cost;
    }
  } while ((clock()/CLOCKS_PER_SEC) - start < stop);
  free(work_elems);
  free(work_subsets);
}

/************* Population-based method *************/

int tournament_selection(int * picked, int T, int * population_cost, int population_size) {
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

// int proportional_selection(int * population_cost, int population_size, int * picked) {
//   int total_cost = 0;
//   for (int i = 0; i < population_size; ++i) {
//     total_cost += population_cost[i];
//   }
//   int dice = ((float)rand()/(float)RAND_MAX) * total_cost;
//   for (int i = 0; i < population_size; ++i) {
//     dice -= population_cost[i];
//     if (dice <= 0) {
//       return i;
//     }
//   }
//   return population_size - 1;
// }

bool is_duplicate(int * elem, int ** list, int elem_size, int list_size) {
  for (int i = 0; i < list_size; ++i) {
    bool all = true;
    for (int j = 0; j < elem_size; ++j) {
      if (elem[j] != list[i][j]) {
        all = false;
      }
    }
    if (all) {
      return true;
    }
  }
  return false;
}

void generate_initial_population(int ** population, int population_size, int * population_cost, int * y) {
  // Generate initial population using random construction search
  for (int i = 0; i < population_size; ++i) {
    population[i] = mymalloc(n * sizeof(int));
    for (int j = 0; j < m; ++j) {
      y[j] = 0;
    }
    construction_search(random_pick_element, random_pick_subset, population[i], y, static_cost);
    perturbative_search("first", population[i], y, total_cost_raw);
    population_cost[i] = total_cost_raw(population[i]);
  }
}

float density_matrix() {
  float total = 0;
  for (int i = 0; i < n; ++i) {
    total += nelement[i];
  }
  return total / (float)(n * m);
}

void genetic_algorithm(int * x, int * y, int t) {
  int population_size = n * density_matrix();
  int T = 10;
  float mutation_rate = 1.0/(float)population_size;
  int ** population = mymalloc(population_size * sizeof(int*));
  int * population_cost = mymalloc(population_size * sizeof(int));
  generate_initial_population(population, population_size, population_cost, y);
  // used in parents selection
  int * picked = mymalloc(population_size * sizeof(int));
  int * children = mymalloc(n * sizeof(int));
  float start = clock()/CLOCKS_PER_SEC, stop = (float)t/1000.0, elapsed;
  do {
    for (int i = 0; i < population_size; ++i) {
      picked[i] = 0;
    }
    // Select parents
    int first_parent = tournament_selection(picked, T, population_cost, population_size);
    int second_parent = tournament_selection(picked, T, population_cost, population_size);
    // recombine parents -> children
    for (int i = 0; i < n; ++i) {
      if (population[first_parent][i] == population[second_parent][i]) {
        children[i] = population[first_parent][i];
      }
      else {
        int first_cost = population_cost[first_parent];
        int second_cost = population_cost[second_parent];
        float p = (float)second_cost / ((float)first_cost + (float)second_cost);
        float dice = (float)rand()/(float)(RAND_MAX);
        if (dice < p) {
          children[i] = population[first_parent][i];
        }
        else {
          children[i] = population[second_parent][i];
        }
      }
    }
    // Mutate children
    for (int i = 0; i < n; ++i) {
      float dice = (float)rand()/(float)(RAND_MAX);
      if (dice < mutation_rate) {
        children[i] = !children[i];
      }
    }
    for (int i = 0; i < m; ++i) {
      y[i] = 0;
    }
    for (int i = 0; i < n; ++i) {
      if (children[i]) {
        add_subset_elems(i, y);
      }
    }
    construction_search(random_pick_element, greedy_pick_subset, children, y, adapted_cover_cost);
    eliminate_redundancy(children, y);
    // replace some parents by children
    if (!is_duplicate(children, population, n, population_size)) {
      int parent_to_replace;
      int mean_cost = 0;
      for (int i = 0; i < population_size; ++i) {
        mean_cost += population_cost[i];
      }
      mean_cost /= population_size;
      do {
        parent_to_replace = rand() % population_size;
      } while (population_cost[parent_to_replace] < mean_cost);
      int * tmp = population[parent_to_replace];
      population[parent_to_replace] = children;
      children = tmp;
      population_cost[parent_to_replace] = total_cost_raw(population[parent_to_replace]);
    }
    elapsed = ((float)clock()/(float)CLOCKS_PER_SEC) - start;
  } while (elapsed < stop);
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
  for (int i = 0; i < population_size; ++i) {
    free(population[i]);
  }
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
  printf("Solution cost : %d\n", total_cost_raw(x));
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
  printf("%d\n", total_cost_raw(x));
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
  // print_instance(0);
  initialize();
  // The four construction searches. The only difference is the cost computation technique
  if (ch1) {
    construction_search(random_pick_element, random_pick_subset, x, y, static_cost);
  }
  else if (ch2) {
    construction_search(random_pick_element, greedy_pick_subset, x, y, static_cost);
  }
  else if (ch3) {
    construction_search(random_pick_element, greedy_pick_subset, x, y, static_cover_cost);
  }
  else if (ch4) {
    construction_search(random_pick_element, greedy_pick_subset, x, y, adapted_cover_cost);
  }
  if (re) {
    eliminate_redundancy(x, y);
  }
  if (fi) {
    perturbative_search("first", x, y, total_cost_raw);
  }
  else if (bi) {
    perturbative_search("best", x, y, total_cost_raw);
  }
  else if (ig) {
    iterated_greedy(x, y, t, 0.5);
  }
  else if (gen) {
    genetic_algorithm(x, y, t);
  }
  // compute_solution_variables();
  // print_solution(x, y);
  // for (int i = 0; i < m; ++i) {
  //   y[i] = 0;
  // }
  // for (int i = 0; i < n; ++i) {
  //   if (x[i]) {
  //     add_subset_elems(i, y);
  //   }
  // }
  // assert(size_elems(y) == m);
  print_cost(x);
  finalize();
  return EXIT_SUCCESS;
}
