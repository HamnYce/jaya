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

void *threaded_loss_func(void *args) {
  pop_t *arg = (pop_t *)args;

  for (int i = arg->start; i < arg->start + span; i++) {
    arg->fit[i] = arg->loss_func(arg->pop_vec[i]);
  }

  pthread_exit(NULL);
}

void calc_fitness(pop_t *pop) {
  pthread_t *ths = malloc(k * sizeof(pthread_t));
  pop->fit = malloc(pop_s * sizeof(double));

  pop_t *pops = malloc(k * sizeof(pop_t));
  for (int i = 0; i < k; i++) {
    pops[i].pop_vec = pop->pop_vec;
    pops[i].fit = pop->fit;
    pops[i].start = i * pop_s / k;
    pops[i].loss_func = pop->loss_func;
    pthread_create(ths + i, NULL, threaded_loss_func, pops + i);
  }

  for (int i = 0; i < k; i++) {
    pthread_join(ths[i], NULL);
  }

  free(ths);
}

// returns a list of the solutions
double *jaya(double (*loss_func)(double *vec)) {
  srand(time(NULL) * 1000);
  double *solutions = malloc(n * sizeof(double));
  pop_t *pop, *mutated_pop;

  span = pop_s / k;
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

double (*loss_func)(double* vec);

int main(int argc, char **argv) {

 if (argc != 5) {
    puts("Please enter 4 arguments to this program:");
    puts("Population");
    puts("Dimension size");
    puts("Max loss function evaluation count");
    puts("K (number of sub-populations)");
    puts("Respectively");
    exit(1);
  }

  pop_s = atoi(argv[1]);
  d = atoi(argv[2]);
  maxfe = atoi(argv[3]);
  k = atoi(argv[4]);
  loss_func = sphere;

  n = maxfe / pop_s;

  double *solution = jaya(sphere);

  char* output_file_name = malloc(100 * sizeof(char));
  sprintf(output_file_name, "output/kpop_pop_s_%i_d_%i.out", pop_s, d);

  FILE *out = fopen(output_file_name, "w");
  for (int i = 0; i < n; i++) {
    fprintf(out, "%10.20f\n", solution[i]);
  }
  fclose(out);

  printf("final:%f\n", solution[n - 1]);

  return 0;
}
