#include <float.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
  double x;
  double y;
} sol_vector;

double rand_double(double min, double max) {
  double r = (double)rand() / (double)RAND_MAX;
  double total_dist = fabs(min) + fabs(max);
  double mapped_r = total_dist * r;
  double offsetted_r = mapped_r - fabs(min);

  return offsetted_r;
}

double loss_function(sol_vector *sol) {
  return pow(sol->x, 2) * pow(sol->y, 2);
}

sol_vector *rand_sol_vector(double min, double max) {
  sol_vector *sol = malloc(sizeof(sol_vector));
  sol->x = rand_double(min, max);
  sol->y = rand_double(min, max);
  return sol;
}

sol_vector *rand_population(double min, double max, int size) {
  sol_vector *population = malloc(sizeof(sol_vector) * size);
  while (--size)
    *(population + size) = *rand_sol_vector(min, max);

  return population;
}

sol_vector *best_worst_sol_vectors(sol_vector *population, int size) {
  double smallest = DBL_MAX;
  double largest = DBL_MIN;
  sol_vector *best_worst_sol = malloc(sizeof(sol_vector) * 2);

  while (--size) {
    if (loss_function(population + size) > largest) {
      largest = loss_function(population + size);
      *(best_worst_sol) = *(population + size);
    } else if (loss_function(population + size) < smallest) {
      largest = loss_function(population + size);
      *(best_worst_sol + 1) = *(population + size);
    }
  }
  return best_worst_sol;
}

sol_vector *mutate_sol_vector(sol_vector *orig_sol,
                              sol_vector *best_worst_sol) {
  sol_vector *mutated_sol = malloc(sizeof(sol_vector));
  sol_vector *r1 = rand_sol_vector(0, 1);
  sol_vector *r2 = rand_sol_vector(0, 1);
  sol_vector *best = best_worst_sol;
  sol_vector *worst = (best_worst_sol + 1);

  double mutated_x = orig_sol->x + r1->x * (best->x - orig_sol->x) +
                     r2->x * (worst->x - orig_sol->x);
  double mutated_y = orig_sol->y + r1->y * (best->y - orig_sol->y) +
                     r2->y * (worst->y - orig_sol->y);

  mutated_sol->x = mutated_x;
  mutated_sol->y = mutated_y;

  return mutated_sol;
}

sol_vector *mutate_population(sol_vector *population,
                              sol_vector *best_worst_sol, int size) {
  sol_vector *mutated_population = malloc(sizeof(sol_vector) * size);

  while (--size) {
    *(mutated_population + size) =
        *mutate_sol_vector((population + size), best_worst_sol);
  }

  return mutated_population;
}

sol_vector *greedy_combine_populations(sol_vector *old_population,
                                       sol_vector *new_population, int size) {

  sol_vector *combined_population = malloc(sizeof(sol_vector) * size);
  while (--size) {
    *(combined_population + size) = loss_function(old_population + size) <
                                            loss_function(new_population + size)
                                        ? *(old_population + size)
                                        : *(new_population + size);
  }

  return combined_population;
}

int main() {
  srand(time(NULL));
  printf("Hello World\n");
  int range[] = {-10, 10};

  int sol_size = 4;
  sol_vector *population = rand_population(-10, 10, 4);

  int n = 10;
  while (n--) {
    sol_vector *best_worst_sol = best_worst_sol_vectors(population, sol_size);
    printf("%f\n", loss_function(best_worst_sol));
    sol_vector *mutated_population =
        mutate_population(population, best_worst_sol, sol_size);
    population = greedy_combine_populations(population, mutated_population, sol_size);
  }

  return 0;
}
