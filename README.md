# Jaya Implementation in C

## How to run
#### With Make
    - make single POP_S=<population_size> D=<dimension> MAXFE<max function eval. count> LOSS=<loss function name>
    - make kpop POP_S=<pop_size> D=<dimension> MAXFE<max func. eval. count> LOSS=<loss function name> K=<sub pop size>

#### With GCC
    - gcc single.c lib/helper.c -o bin/single
        - bin/single <pop size> <dimensions> <maxfe> <loss function name>
    - gcc kpop.c lib/helper.c -o bin/single 
        - bin/kpop <pop size> <dimensions> <maxfe> <loss function name> <sub pop size>


Note: kpop does not currently work with the new changes to pop_t. please stay tuned for any new changes via whatsapp DMs.
