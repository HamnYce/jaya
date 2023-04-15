#include "helper.h"
#include <float.h>
#include <math.h>
#include <stdlib.h>


void free_vec(double *vec) { free(vec); }

void free_pop(double **pop) {
  for (int i = 0; i < pop_s; i++)
    free_vec(pop[i]);

  free(pop);
}

void free_pop_t(pop_t *pop) {
  free_pop(pop->pop_vec);
  free_vec(pop->fit);
  free(pop);
}

double *copy_vec(double *vec) {
  double *vec_copy = malloc(d * sizeof(vec));

  for (int i = 0; i < d; i++)
    vec_copy[i] = vec[i];

  return vec_copy;
}

double *rand_vec(double min, double max) {
  double *vec = malloc(d * sizeof(double));

  for (int i = 0; i < d; i++)
    vec[i] = rand_double(min, max);

  return vec;
}

void rand_pop(pop_t *pop) {
  for (int i = 0; i < pop_s; i++) {
    for (int j = 0; j < d; j++) {
      pop->pop_vec[i][j] = rand_double(-B, B);
    }
  }
}

void init_pop(pop_t *pop, double (*loss_func)(double *vec)) {
  pop->pop_vec = malloc(pop_s * sizeof(double *));

  for (int i = 0; i < pop_s; i++) {
    pop->pop_vec[i] = malloc(d * sizeof(double));
  }

  pop->fit = malloc(pop_s * sizeof(double));
}

double sphere(double *vec) {
  double res = 0;
  for (int i = 0; i < d; i++)
    res += pow(vec[i], 2);

  return res;
}

double rosenbrock(double *vec) {
  double res = 0;
  for (int i = 0; i < d - 1; i++)
    res += 100 * pow(vec[i + 1] - pow(vec[i], 2), 2) + pow(1 - vec[i], 2);

  return res;
}

double rastrigin(double *vec) {
  double res = 0;
  for (int i = 0; i < d; i++)
    res += pow(vec[i], 2) - 10 * cos(2 * M_PI * vec[i]) + 10;

  return res;
}


double rand_double(double min, double max) {
  double range = max - min;
  double rand_num = (double)rand() / RAND_MAX;
  double rand_actual = rand_num * range + min;

  return rand_num * range + min;
}

void find_best_worst(pop_t *pop) {
  double best = DBL_MAX;
  double worst = -DBL_MAX;

  int best_i = 0;
  int worst_i = 0;

  for (int i = 0; i < pop_s; i++) {
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

void mutate(pop_t *pop, pop_t *mutated_pop, int vec_i) {
  double *r1 = rand_vec(0, 1);
  double *r2 = rand_vec(0, 1);

  for (int i = 0; i < d; i++) {
    mutated_pop->pop_vec[vec_i][i] =
        pop->pop_vec[vec_i][i] +
        (r1[i] * (pop->pop_vec[pop->best][i] - pop->pop_vec[vec_i][i])) -
        (r2[i] * (pop->pop_vec[pop->worst][i] - pop->pop_vec[vec_i][i]));
  }

  free(r1);
  free(r2);
}

void mutate_pop(pop_t *pop, pop_t *mutated_pop) {
  for (int i = 0; i < pop_s; i++)
    mutate(pop, mutated_pop, i);
}

void combine_into_pop(pop_t *pop1, pop_t *pop2) {

  // TODO: DRY this up
  pop1->best =
      pop1->fit[pop1->best] <= pop2->fit[pop2->best] ? pop1->best : pop2->best;

  pop1->worst = pop1->fit[pop1->worst] <= pop2->fit[pop2->worst] ? pop1->worst
                                                                 : pop2->worst;

  for (int i = 0; i < pop_s; i++) {
    if (pop1->fit[i] > pop2->fit[i]) {
      for (int j = 0; j < d; j++) {
        pop1->pop_vec[i][j] = pop2->pop_vec[i][j];
      }

      pop1->fit[i] = pop2->fit[i];
    }

  }
}
