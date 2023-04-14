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

double *jaya(double (*loss_func)(double *vec), int n);

void calc_fitness(pop_t *pop) {
  for (int i = 0; i < POP_S; i++) {
    pop->fit[i] = pop->loss_func(pop->pop_vec[i]);
  }
}

// returns a list of the solutions
double *jaya(double (*loss_func)(double *vec), int n) {
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

  // the loss function is carried on each member of a population x times
  // then on the mutated version of that population.
  // therefore it is carried out POP_S * 2 times per iteration
  // FE == 2 * POP_S * (n)
  // therefore n = FE / (2 * POP_S)

  int n = MAXFE / (2 * POP_S);
  double *solution = jaya(sphere, n);

  FILE *out = fopen("output/single.out", "w");
  for (int i = 0; i < n; i++) {
    fprintf(out, "%10.20f\n", solution[i]);
  }
  fclose(out);

  printf("final:%f\n", solution[n - 1]);
  return 0;
}
