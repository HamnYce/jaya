#include "lib/helper.h"
#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

void fitness(pop_t *pop);
void find_best_worst(pop_t *pop);

void mutate_pop(pop_t *pop, pop_t *mutated_pop);
void combine_into_pop(pop_t *pop1, pop_t *pop2);

double *jaya(double (*loss_func)(double *vec), int n);

void *threaded_loss_func(void *args) {
  pop_t *arg = (pop_t *)args;

  for (int i = arg->start; i < arg->start + SPAN; i++) {
    arg->fit[i] = arg->loss_func(arg->pop_vec[i]);
  }

  pthread_exit(NULL);
}

void calc_fitness(pop_t *pop) {
  pthread_t *ths = malloc(K * sizeof(pthread_t));
  pop->fit = malloc(POP_S * sizeof(double));

  pop_t *pops = malloc(K * sizeof(pop_t));
  for (int i = 0; i < K; i++) {
    pops[i].pop_vec = pop->pop_vec;
    pops[i].fit = pop->fit;
    pops[i].start = i * POP_S / K;
    pops[i].loss_func = pop->loss_func;
    pthread_create(ths + i, NULL, threaded_loss_func, pops + i);
  }

  for (int i = 0; i < K; i++) {
    pthread_join(ths[i], NULL);
  }

  free(ths);
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
  calc_fitness(pop);
  find_best_worst(pop);

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

  // the loss function is calculated once for every vector
  // in the population during the mutation step

  int n = MAXFE / POP_S;
  double *solution = jaya(sphere, n);

  FILE *out = fopen("output/kpop.out", "w");
  for (int i = 0; i < n; i++) {
    fprintf(out, "%10.20f\n", solution[i]);
  }
  fclose(out);

  printf("final:%f\n", solution[n - 1]);
  return 0;
}
