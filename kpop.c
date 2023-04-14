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

void init_pop(pop_t *pop, double (*loss_func)(double *vec)) {
  pop->pop_vec = malloc(POP_S * sizeof(double *));

  for (int i = 0; i < POP_S; i++) {
    pop->pop_vec[i] = malloc(D * sizeof(double));
  }

  pop->fit = malloc(POP_S * sizeof(double));
  pop->loss_func = loss_func;
}

void *threaded_rand_vec(void *argv) {
  pop_t *pop = (pop_t *)argv;

  for (int i = pop->start; i < pop->start + SPAN; i++) {
    for (int j = 0; j < D; j++) {
      pop->pop_vec[i][j] = rand_double(-B, B);
    }
  }

  pthread_exit(NULL);
}

void rand_pop(pop_t *pop) {
  pthread_t *ths = malloc(K * sizeof(pthread_t));

  pop_t *pops = malloc(K * sizeof(pop_t));
  for (int i = 0; i < K; i++) {
    pops[i].pop_vec = pop->pop_vec;
    pops[i].start = i * SPAN;
    pthread_create(ths + i, NULL, threaded_rand_vec, pops + i);
  }

  for (int i = 0; i < K; i++) {
    pthread_join(ths[i], NULL);
  }

  free(ths);
  free(pops);
}

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

void mutate_pop(pop_t *pop, pop_t *mutated_pop) {
  for (int i = 0; i < POP_S; i++)
    mutate(pop, mutated_pop, i);
}

void set_pop(pop_t *pop, pop_t *better_pop, pop_t *worse_pop, int vec_i) {
  for (int i = 0; i < D; i++) {
    pop->pop_vec[vec_i][i] = better_pop->pop_vec[vec_i][i];
  }

  pop->fit[vec_i] = better_pop->fit[vec_i];
}

void combine_into_pop(pop_t *pop1, pop_t *pop2) {

  // TODO: DRY this up
  pop1->best =
      pop1->fit[pop1->best] <= pop2->fit[pop2->best] ? pop1->best : pop2->best;

  pop1->worst = pop1->fit[pop1->worst] <= pop2->fit[pop2->worst] ? pop1->worst
                                                                 : pop2->worst;

  for (int i = 0; i < POP_S; i++) {
    if (pop1->fit[i] <= pop2->fit[i])
      set_pop(pop1, pop1, pop2, i);
    else
      set_pop(pop1, pop2, pop1, i);
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

  FILE *out = fopen("output/kpop.out", "w");
  for (int i = 0; i < n; i++) {
    fprintf(out, "%10.20f\n", solution[i]);
  }
  fclose(out);

  printf("final:%f\n", solution[n - 1]);
  return 0;
}
