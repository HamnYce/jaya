#include <stdlib.h>
#include <math.h>
#include "helper.h"
#include <float.h>

void free_vec(double *vec) { free(vec); }

void free_pop(double **pop) {
  for (int i = 0; i < POP_S; i++)
    free_vec(pop[i]);

  free(pop);
}

void free_pop_t(pop_t* pop) {
  free_pop(pop->pop_vec);
  free_vec(pop->fit);
  free(pop);
}

double *copy_vec(double *vec) {
  double *vec_copy = malloc(D * sizeof(vec));

  for (int i = 0; i < D; i++)
    vec_copy[i] = vec[i];

  return vec_copy;
}

double *rand_vec(double min, double max) {
  double *vec = malloc(D * sizeof(double));

  for (int i = 0; i < D; i++)
    vec[i] = rand_double(min, max);

  return vec;
}

double sphere(double *vec) {
  double res = 0;
  for (int i = 0; i < D; i++)
    res += pow(vec[i], 2);

  return res;
}

double rosenbrock(double *vec) {
  double res = 0;
  for (int i = 0; i < D - 1; i++)
    res += 100 * pow(vec[i + 1] - pow(vec[i], 2), 2) + pow(1 - vec[i], 2);

  return res;
}

double rastrigin(double *vec) {
  double res = 0;
  for (int i = 0; i < D; i++)
    res += pow(vec[i], 2) - 10 * cos(2 * M_PI * vec[i]) + 10;

  return res;
}

double rand_double(double min, double max) {
  double r = (double)rand() / (double)RAND_MAX;
  double total_dist = fabs(min) + fabs(max);
  double mapped_r = total_dist * r;
  double offsetted_r = mapped_r - fabs(min);

  return offsetted_r;
}

void find_best_worst(pop_t *pop) {
  double best = DBL_MAX;
  double worst = -DBL_MAX;

  int best_i = 0;
  int worst_i = 0;

  for (int i = 0; i < POP_S; i++) {
    if (pop->fit[i] <= best) {
      best_i = i;
      best = pop->fit[i];
    }

    if (pop->fit[i] >= worst) {
      worst_i = i;
      worst = pop->fit[i];
    }
  }

  pop->best = best_i;
  pop->worst = worst_i;
}
