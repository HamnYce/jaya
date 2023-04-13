#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DIM 100
#define POP_SIZE 100
#define DEBUG 1
#define ITERS 100

double rand_double(double min, double max) {
  double r = (double)rand() / (double)RAND_MAX;
  double total_dist = fabs(min) + fabs(max);
  double mapped_r = total_dist * r;
  double offsetted_r = mapped_r - fabs(min);

  return offsetted_r;
}

typedef struct {
  double *x; // values of vector
} vector_t;

void free_vector_t(vector_t *vec) {
  free(vec->x);
  free(vec);
}

void free_population(vector_t **pop) {
  for (int i = 0; i < POP_SIZE; i++) {
    free_vector_t(*(pop + i));
  }
  free(pop);
}

double sphere_loss_function(vector_t *vec) {
  double total = 0;

  for (int i = 0; i < DIM; i++) {
    total = pow(*(vec->x + i), 2);
  }

  return total;
}

double rosenbrock_loss_function(vector_t *vec) {
  double total = 0;

  for (int i = 0; i < DIM - 1; i++) {
    total += pow(100 * (*(vec->x + i + 1) - pow(*(vec->x + i), 2)), 2) +
             pow((1 - *(vec->x + i)), 2);
  }

  return total;
}

vector_t *inited_vector_t() {
  vector_t *init_vec = malloc(sizeof(vector_t));

  init_vec->x = malloc(sizeof(double) * DIM);

  for (int i = 0; i < DIM; i++) {
    *(init_vec->x + i) = 0;
  }

  return init_vec;
}

vector_t *copy_vector_t(vector_t *old) {
  vector_t *copy_vec = inited_vector_t();

  for (int i = 0; i < DIM; i++) {
    *(copy_vec->x + i) = *(old->x + i);
  }

  return copy_vec;
}

vector_t *rand_vector_t(double min, double max) {
  vector_t *r_vector = inited_vector_t();

  for (int i = 0; i < DIM; i++) {
    *(r_vector->x + i) = rand_double(min, max);
  }

  return r_vector;
}

vector_t **best_worst_vector(vector_t **pop) {
  double smallest = DBL_MAX;
  double largest = DBL_MIN;

  vector_t **best_worst_vecs = malloc(sizeof(vector_t *) * 2);

  for (int i = 0; i < POP_SIZE; i++) {
    // double loss = sphere_loss_function(*(pop + i));
    double loss = sphere_loss_function(*(pop + i));

    if (loss < smallest) {
      smallest = loss;
      *(best_worst_vecs) = *(pop + i);
    }
    if (loss > largest) {
      largest = loss;
      *(best_worst_vecs + 1) = *(pop + i);
    }
  }

  return best_worst_vecs;
}

vector_t *mutate_vector_t(vector_t *orig, vector_t **best_worst_vecs) {
  vector_t *mutated_vec = inited_vector_t();

  vector_t *r1 = rand_vector_t(0, 1);
  vector_t *r2 = rand_vector_t(0, 1);

  vector_t *best = *best_worst_vecs;
  vector_t *worst = *(best_worst_vecs + 1);

  for (int i = 0; i < DIM; i++) {
    *(mutated_vec->x + i) = *(orig->x + i) +
                            *(r1->x + i) * (*(best->x + i) - *(orig->x + i)) -
                            *(r2->x + i) * (*(worst->x + i) - *(orig->x + i));
  }

  free_vector_t(r1);
  free_vector_t(r2);

  return mutated_vec;
}

vector_t **rand_population(double min, double max) {
  vector_t **population = malloc(sizeof(vector_t *) * POP_SIZE);

  for (int i = 0; i < POP_SIZE; i++) {
    *(population + i) = rand_vector_t(min, max);
  }

  return population;
}

vector_t **mutate_population(vector_t **pop, vector_t **best_worst_vecs) {
  vector_t **mutated_pop = malloc(sizeof(vector_t *) * POP_SIZE);

  for (int i = 0; i < POP_SIZE; i++) {
    *(mutated_pop + i) = mutate_vector_t(*(pop + i), best_worst_vecs);
  }

  return mutated_pop;
}

vector_t **greedy_combine_population(vector_t **old, vector_t **new) {
  vector_t **combined_population = malloc(sizeof(vector_t *) * POP_SIZE);

  for (int i = 0; i < POP_SIZE; i++) {
    *(combined_population + i) = copy_vector_t(
        sphere_loss_function(*(old + i)) < sphere_loss_function(*(new + i))
            ? *(old + i)
            : *(new + i));
  }

  free_population(old);
  free_population(new);

  return combined_population;
}

int main() {
  srand(time(NULL));
  int range[] = {-30, 30};
  int n = ITERS;

  int sol_size = 4;
  vector_t **population = rand_population(range[0], range[1]);
  vector_t **best_worst_vecs;

  while (n--) {
    best_worst_vecs = best_worst_vector(population);
    if (DEBUG) {
      printf("%i: %.10f %.10f\n", n, sphere_loss_function(*(best_worst_vecs)),
             sphere_loss_function(*(best_worst_vecs + 1)));
    }
    vector_t **mutated_pop = mutate_population(population, best_worst_vecs);

    population = greedy_combine_population(population, mutated_pop);
  }
  return 0;
}
