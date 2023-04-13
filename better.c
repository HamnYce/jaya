#include <float.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

// make these arguments
#define D 1000
#define POP_S 1000
#define MAXFE 1000000
#define LOWER_B -10.0
#define UPPER_B 10.0

void free_vec(double *vec);
void free_pop(double **pop);

double sphere(double *vec);
double rosenbrock(double *vec);
double rastrigin(double *vec);

double rand_double(double min, double max);

double *copy_vec(double *vec);
double *rand_vec(double min, double max);
double *mutate(double *, double *fit_range);
double *range(double *fit);
double *fitness(double (*loss)(double *vec), double **pop);

double **rand_pop();
double **mutate_pop(double **pop, double *fit_range);
double **combine(double **pop1, double **pop2, double *fit1, double *fit2);

double **jaya(double (*loss_func)(double *vec), int n);

void free_vec(double *vec) { free(vec); }

void free_pop(double **pop) {
  for (int i = 0; i < POP_S; i++)
    free_vec(pop[i]);

  free(pop);
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

// [solution vectors]
double **rand_pop() {
  double **pop = malloc(POP_S * sizeof(double *));

  for (int i = 0; i < POP_S; i++)
    pop[i] = rand_vec(LOWER_B, UPPER_B);

  return pop;
}

// [solution vectors] -> [fits]
double *fitness(double (*loss_func)(double *vec), double **pop) {
  double *fit = malloc(POP_S * sizeof(double));

  for (int i = 0; i < POP_S; i++)
    fit[i] = loss_func(pop[i]);

  return fit;
}

// [fits] -> [lowest fit, greatest fit]
double *range(double *fit) {
  double *range = malloc(2 * sizeof(double));

  range[0] = DBL_MAX;
  range[1] = DBL_MIN;

  for (int i = 0; i < POP_S; i++) {
    if (fit[i] <= range[0]) {
      range[0] = fit[i];
    }

    if (fit[i] > range[1]) {
      range[1] = fit[i];
    }
  }
  return range;
}

// solution_vector -> mutated_solution_vector
double *mutate(double *vec, double *fit_range) {
  double *mutated_vec = malloc(D * sizeof(double));

  double *r1 = rand_vec(0, 1);
  double *r2 = rand_vec(0, 1);

  for (int i = 0; i < D; i++) {
    mutated_vec[i] = vec[i] + (r1[i] * (fit_range[0] - vec[i])) -
                     (r2[i] * (fit_range[1] - vec[i]));
  }

  return mutated_vec;
}

// [solutions vectors], [fits] -> [list of mutated solutions vectors]
double **mutate_pop(double **pop, double *fit) {
  double **mutated_pop = malloc(POP_S * sizeof(double **));

  for (int i = 0; i < POP_S; i++)
    mutated_pop[i] = mutate(pop[i], fit);

  return mutated_pop;
}

// [solution vectors], [solution vectors], [fits], [fits] -> [solution vectors]
double **combine(double **pop1, double **pop2, double *fit1, double *fit2) {
  double **combined_pop = malloc(POP_S * sizeof(double **));

  for (int i = 0; i < POP_S; i++) {
    if (fit1[i] < fit2[i])
      combined_pop[i] = copy_vec(pop1[i]);
    else
      combined_pop[i] = copy_vec(pop2[i]);
  }

  return combined_pop;
}

// returns a list of the solutions
double **jaya(double (*loss_func)(double *vec), int n) {
  double **solutions = malloc(n * sizeof(double));
  double **pop, **mutated_pop, **new_pop;
  double *fit, *mutated_fit, *fit_range, *mutated_fit_range;

  pop = rand_pop();

  for (int i = 0; i < n; i++) {
    fit = fitness(loss_func, pop);
    fit_range = range(fit);

    mutated_pop = mutate_pop(pop, fit_range);
    mutated_fit = fitness(loss_func, mutated_pop);
    mutated_fit_range = range(mutated_fit);

    new_pop = combine(pop, mutated_pop, fit, mutated_fit);

    solutions[i] = fit_range;

    free_vec(fit);
    free_vec(mutated_fit);
    free_pop(pop);
    free_pop(mutated_pop);

    pop = new_pop;
  }
  return solutions;
}

int main(int argc, char **argv) {
  srand(time(NULL));

  // we will use the maximumFunction evaluation, D and the population size to
  // dictate the number of iterations we will do.
  // the loss function is carried on each member of a population x times
  // then on the mutated version of that population.
  // therefore it is carried out POP_S * 2 times per iteration
  // FE == 2 * POP_S * (n)
  // therefore n = FE / (2 * POP_S)
  int n = MAXFE / (2 * POP_S);
  double **solutions = jaya(sphere, n);

  for (int i = n - 10; i < n; i++) {
    printf("best:%f worst:%f\n", solutions[i][0], solutions[i][1]);
  }

  printf("best:%f worst:%f\n", solutions[n - 1][0], solutions[n - 1][1]);
  return 0;
}
