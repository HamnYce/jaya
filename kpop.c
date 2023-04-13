#include <float.h>
#include <math.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "lib/helper.h"

double *mutate(double *, double **best_worst);
double *fitness(double (*loss)(double *vec), double **pop);
double **get_best_worst(double **pop, double *fit);

double **mutate_pop(double **pop, double **best_worst);
double **combine(double **pop1, double **pop2, double *fit1, double *fit2);

double *jaya(double (*loss_func)(double *vec), int n);

void *threaded_rand_vec(void *argv) {
  pop_t *arg = (pop_t *)argv;

  for (int i = arg->start; i < arg->start + SPAN; i++) {
    arg->pop_vec[i] = rand_vec(-B, B);
  }

  pthread_exit(NULL);
}

double **threaded_rand_pop() {
  pthread_t *ths = malloc(K * sizeof(pthread_t));
  double **pop_vec = malloc(POP_S * sizeof(double *));

  for (int i = 0; i < K; i++) {
    pop_t pop;
    pop.pop_vec = pop_vec;
    pop.start = i * POP_S / K;
    pthread_create(ths + i, NULL, threaded_rand_vec, &pop);
  }

  for (int i = 0; i < K; i++) {
    pthread_join(ths[i], NULL);
  }

  free(ths);
  return pop_vec;
}

void* threaded_loss_func(void* args) {
  pop_t *arg = (pop_t*) args;

  for (int i = arg->start; i < arg->start + SPAN; i++) {
    arg->fit[i] = arg->loss_func(arg->pop_vec[i]);
  }

  pthread_exit(NULL);
}

double* threaded_fitness(double (*loss_func)(double *vec), double** pop_vec) {
  pthread_t *ths = malloc(K * sizeof(pthread_t));
  double *fit = malloc(POP_S * sizeof(double));

  for (int i = 0; i < K; i++) {
    pop_t pop;
    pop.pop_vec = pop_vec;
    pop.fit = fit;
    pop.start = i * POP_S / K;
    pop.loss_func = loss_func;
    pthread_create(ths + i, NULL, threaded_loss_func, &pop);
  }

  for (int i = 0; i < K; i++) {
    pthread_join(ths[i], NULL);
  }

  free(ths);
  return fit;
}

// [solution vectors] -> [fits]
double *fitness(double (*loss_func)(double *vec), double **pop) {
  double *fit = malloc(POP_S * sizeof(double));

  for (int i = 0; i < POP_S; i++) {
    fit[i] = loss_func(pop[i]);
  }

  return fit;
}

// [fits] -> [lowest fit, greatest fit]
double **get_best_worst(double **pop, double *fit) {
  double **best_worst = malloc(2 * sizeof(double *));

  double best = DBL_MAX;
  double worst = -DBL_MAX;

  for (int i = 0; i < POP_S; i++) {
    if (fit[i] <= best) {
      best = fit[i];
      best_worst[0] = copy_vec(pop[i]);
    }

    if (fit[i] >= worst) {
      worst = fit[i];
      best_worst[1] = copy_vec(pop[i]);
    }
  }
  return best_worst;
}

// solution_vector -> mutated_solution_vector
double *mutate(double *vec, double **best_worst) {
  double *mutated_vec = malloc(D * sizeof(double));

  double *r1 = rand_vec(0, 1);
  double *r2 = rand_vec(0, 1);

  for (int i = 0; i < D; i++) {
    mutated_vec[i] = vec[i] + (r1[i] * (best_worst[0][i] - vec[i])) -
                     (r2[i] * (best_worst[1][i] - vec[i]));
  }

  return mutated_vec;
}

// [solutions vectors], [fits] -> [list of mutated solutions vectors]
double **mutate_pop(double **pop, double **best_worst) {
  double **mutated_pop = malloc(POP_S * sizeof(double **));

  for (int i = 0; i < POP_S; i++)
    mutated_pop[i] = mutate(pop[i], best_worst);

  return mutated_pop;
}

// [solution vectors], [solution vectors], [fits], [fits] -> [solution vectors]
double **combine(double **pop1, double **pop2, double *fit1, double *fit2) {
  double **combined_pop = malloc(POP_S * sizeof(double **));

  for (int i = 0; i < POP_S; i++) {
    if (fit1[i] <= fit2[i])
      combined_pop[i] = copy_vec(pop1[i]);
    else
      combined_pop[i] = copy_vec(pop2[i]);
  }

  return combined_pop;
}

// returns a list of the solutions
double *jaya(double (*loss_func)(double *vec), int n) {
  srand(time(NULL) * 1000);
  double *solutions = malloc(n * sizeof(double));
  double **pop, **mutated_pop, **new_pop, **best_worst, **mutated_best_worst;
  double *fit, *mutated_fit;

  // pop = rand_pop();

  for (int i = 0; i < n; i++) {
    // fit = fitness(loss_func, pop);
    fit = threaded_fitness(loss_func, pop);
    best_worst = get_best_worst(pop, fit);

    mutated_pop = mutate_pop(pop, best_worst);
    mutated_fit = fitness(loss_func, mutated_pop);
    mutated_best_worst = get_best_worst(mutated_pop, mutated_fit);

    new_pop = combine(pop, mutated_pop, fit, mutated_fit);

    solutions[i] = loss_func(best_worst[0]);

    free_vec(fit);
    free_vec(mutated_fit);
    free_pop(pop);
    free_pop(mutated_pop);

    pop = new_pop;
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

  printf("best:%f worst:%f\n", solution[n - 1], solution[n - 1]);
  return 0;
}
