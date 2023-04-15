#include "lib/helper.h"
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

void mutate(pop_t *pop, pop_t *mutated_pop, int i);
void fitness(pop_t *pop);
void find_best_worst(pop_t *pop);

void mutate_pop(pop_t *pop, pop_t *mutated_pop);
void combine_into_pop(pop_t *pop1, pop_t *pop2);

void calc_fitness(pop_t *pop) {
  for (int i = 0; i < pop_s; i++) {
    pop->fit[i] = loss_func(pop->pop_vec[i]);
  }
}

// returns a list of the solutions
double *jaya() {
  char *output_file_name = malloc(100 * sizeof(char));
  sprintf(output_file_name, "output/single_pop_s_%i_d_%i.out", pop_s, d);
  FILE *out = fopen(output_file_name, "w");

  srand(time(NULL) * 1000);

  double *best_fits = malloc(n * sizeof(double));
  double **best_solutions = malloc(pop_s * sizeof(double *));

  pop_t *pop, *mutated_pop;

  pop = malloc(sizeof(pop_t));
  mutated_pop = malloc(sizeof(pop_t));

  init_pop(pop, loss_func);
  init_pop(mutated_pop, loss_func);

  rand_pop(pop);
  find_best_worst(pop);
  calc_fitness(pop);

  for (int i = 0; i < n; i++) {
    mutate_pop(pop, mutated_pop);

    calc_fitness(mutated_pop);

    find_best_worst(mutated_pop);

    combine_into_pop(pop, mutated_pop);

    best_fits[i] = pop->fit[pop->best];

    for (int j = 0; j < n; j++) {
      fprintf(out, "%f ", pop->pop_vec[pop->best][j]);
    }
    fprintf(out, "%10.20f\n", best_fits[i]);
    
  }

  fclose(out);
  return best_fits;
}

int main(int argc, char **argv) {
  if (argc != 5) {
    puts("Please enter 4 arguments to this program");
    puts("population size");
    puts("dimension size");
    puts("max FE count");
    puts("loss function name ('sphere', 'rosenbrock', or 'rastrigin')");
    puts("respectively");
    exit(1);
  }

  pop_s = atoi(argv[1]);
  d = atoi(argv[2]);
  maxfe = atoi(argv[3]);
  char *loss_func_s = argv[4];
  n = maxfe / pop_s;

  if (strcmp(loss_func_s, "sphere"))
    loss_func = sphere;
  else if (strcmp(loss_func_s, "rosenbrock"))
    loss_func = rosenbrock;
  else if (strcmp(loss_func_s, "rastrigin"))
    loss_func = rastrigin;
  else
    loss_func = sphere;

  double *solution = jaya();

  printf("final:%f\n", solution[n - 1]);
  return 0;
}
