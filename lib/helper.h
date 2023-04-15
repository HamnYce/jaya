#ifndef HELPER
#define HELPER

#define B 30.0
extern int pop_s, d, span, k, n, maxfe;
extern double (*loss_func)(double *vec);

// TODO: fix this because naming is kinda werid
// capital is like macro
typedef struct {
  double **pop_vec;
  double *fit;
  int best;
  int worst;
  int start;
} pop_t;

double rand_double(double min, double max);

double sphere(double *vec);
double rosenbrock(double *vec);
double rastrigin(double *vec);

double *copy_vec(double *vec);
double *rand_vec(double min, double max);

void free_vec(double *vec);
void free_pop(double **pop_vec);
void free_pop_t(pop_t *pop);

void rand_pop(pop_t *pop);
void init_pop(pop_t *pop);

void mutate_pop(pop_t *pop, pop_t *mutated_pop);
void mutate(pop_t *pop, pop_t *mutated_pop, int vec_i);

void find_best_worst(pop_t *pop);

void combine_into_pop(pop_t *pop1, pop_t *pop2);
void set_pop(pop_t *pop, pop_t *better_pop, pop_t *worse_pop, int vec_i);

double *jaya();
#endif
