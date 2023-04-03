#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define DIM 2
#define POP_SIZE 4

double rand_double(double min, double max) {
  double r = (double)rand() / (double)RAND_MAX;
  double total_dist = fabs(min) + fabs(max);
  double mapped_r = total_dist * r;
  double offsetted_r = mapped_r - fabs(min);

  return offsetted_r;
}

typedef struct {
  double *x; // values of vector
} n_vector;

void free_n_vector(n_vector *vec) {
  free(vec->x);
  free(vec);
}

void free_population(n_vector **pop) {
  for (int i = 0; i < POP_SIZE; i++) {
    free_n_vector(*(pop + i));
  }
  free(pop);
}

double sphere_loss_function(n_vector *vec) {
  return pow(*(vec->x), 2) + pow(*(vec->x + 1), 2);
}

n_vector *inited_n_vector() {
  n_vector *init_vec = malloc(sizeof(n_vector));

  init_vec->x = malloc(sizeof(double) * DIM);

  for (int i = 0; i < DIM; i++) {
    *(init_vec->x + i) = 0;
  }

  return init_vec;
}

n_vector *rand_n_vector(double min, double max) {
  n_vector *r_vector = inited_n_vector();

  for (int i = 0; i < DIM; i++) {
    *(r_vector->x + i) = rand_double(min, max);
  }

  return r_vector;
}

n_vector **best_worst_n_vectors(n_vector **pop) {
  double smallest = DBL_MAX;
  double largest = DBL_MIN;

  n_vector **best_worst_vecs = malloc(sizeof(n_vector *) * 2);

  for (int i = 0; i < POP_SIZE; i++) {
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

n_vector *mutate_n_vector(n_vector *orig, n_vector **best_worst_vecs) {
  n_vector *mutated_vec = inited_n_vector();

  n_vector *r1 = rand_n_vector(0, 1);
  n_vector *r2 = rand_n_vector(0, 1);

  n_vector *best = *best_worst_vecs;
  n_vector *worst = *(best_worst_vecs + 1);

  for (int i = 0; i < DIM; i++) {
    *(mutated_vec->x + i) = *(orig->x + i) +
                            *(r1->x + i) * (*(best->x + i) - *(orig->x + i)) +
                            *(r2->x + i) * (*(worst->x + i) - *(orig->x + i));
  }

  free_n_vector(r1);
  free_n_vector(r2);

  return mutated_vec;
}

n_vector **rand_population(double min, double max) {
  n_vector **population = malloc(sizeof(n_vector *) * POP_SIZE);

  for (int i = 0; i < POP_SIZE; i++) {
    *(population + i) = rand_n_vector(min, max);
  }

  return population;
}

n_vector **mutate_population(n_vector **pop, n_vector **best_worst_vecs) {
  n_vector **mutated_pop = malloc(sizeof(n_vector *) * POP_SIZE);

  for (int i = 0; i < POP_SIZE; i++) {
    *(mutated_pop + i) = mutate_n_vector(*(pop + i), best_worst_vecs);
  }

  return mutated_pop;
}

n_vector **greedy_combine_population(n_vector **old, n_vector **new) {
  n_vector **combined_population = malloc(sizeof(n_vector *) * POP_SIZE);

  for (int i = 0; i < POP_SIZE; i++) {
    *(combined_population + i) =
        sphere_loss_function(*(old + i)) < sphere_loss_function(*(new + i))
            ? *(old + i)
            : *(new + i);
  }

  return combined_population;
}

int main() {
  srand(time(NULL));
  int range[] = {-10, 10};
  int n = 100;

  int sol_size = 4;
  n_vector **population = rand_population(range[0], range[1]);
  n_vector **best_worst_vecs;

  while (n--) {
    best_worst_vecs = best_worst_n_vectors(population);
    if (1) {
      printf("%i: %.10f %.10f\n", n, sphere_loss_function(*(best_worst_vecs)),
             sphere_loss_function(*(best_worst_vecs + 1)));
    }
    n_vector **mutated_pop = mutate_population(population, best_worst_vecs);

    population = greedy_combine_population(population, mutated_pop);
  }
  return 0;
}
