#ifndef HELPER

#define D 2
#define POP_S 100
#define K 5
#define MAXFE 10000000
#define B 30.0
#define SPAN POP_S / K

typedef struct {
  double **pop_vec;
  double *fit;
  int start;
  int best;
  int worst;
  double (*loss_func)(double* vec);
} pop_t;

void free_vec(double* vec);
double*copy_vec(double* vec);
void free_pop(double** pop_vec);
void free_pop_t(pop_t* pop);

double* rand_vec(double min, double max);

void find_best_worst(pop_t* pop);
void mutate(pop_t *pop, pop_t *mutated_pop, int vec_i);

double sphere(double *vec);
double rosenbrock(double *vec);
double rastrigin(double *vec);

double rand_double(double min, double max);

#endif
