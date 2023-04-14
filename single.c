#include "lib/helper.h"
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>


void mutate(pop_t *pop, pop_t *mutated_pop, int i);
void fitness(pop_t *pop);
void find_best_worst(pop_t *pop);

void mutate_pop(pop_t *pop, pop_t *mutated_pop);
void combine_into_pop(pop_t *pop1, pop_t *pop2);


void calc_fitness(pop_t *pop) {
  for (int i = 0; i < pop_s; i++) {
    pop->fit[i] = pop->loss_func(pop->pop_vec[i]);
  }
}

// returns a list of the solutions
double *jaya(double (*loss_func)(double *vec)) {
  srand(time(NULL) * 1000);
  double *solutions = malloc(n * sizeof(double));
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

    solutions[i] = pop->fit[pop->best];
  }

  return solutions;
}

int main(int argc, char **argv) {
  if (argc != 4) {
    puts("Please enter 3 arguments to this program");
    puts("population & dimension size and max loss function evaluation count");
    puts("respectively");
    exit(1);
  }

  pop_s = atoi(argv[1]);
  d = atoi(argv[2]);
  maxfe = atoi(argv[3]);
  n = maxfe / pop_s;

  double *solution = jaya(sphere);

  char* output_file_name = malloc(100 * sizeof(char));
  sprintf(output_file_name, "output/single_pop_s_%i_d_%i.out", pop_s, d);

  FILE *out = fopen(output_file_name, "w");
  for (int i = 0; i < n; i++) {
    fprintf(out, "%10.20f\n", solution[i]);
  }
  fclose(out);

  printf("final:%f\n", solution[n - 1]);
  return 0;
}
