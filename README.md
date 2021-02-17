# Temps Reel
## Exercise 1 - Runners 
---
### 1. Intro
The aim of this assignment is to implement the Barrier multithreading synchronisation technique.

The program is launched with the amount of laps to run in parameter.

Several threads are created, then each of them sleeps for a random amount of time, then reaches
the barrier, where it will wait for the other runners.

Once all runners have reached the barrier,
all of them will start a new lap.

The code can be found on [the GitHub repository](https://github.com/gilleshenrard/ITLG_temps-reel/tree/assignment1)

Use :
```shell
    bin/runners nb_runners nb_laps
```

### 2. Current features
* Threads synchronisation functions :
```C
    int barrier_alloc(barrier_t** bar, const uint16_t nb);
    int barrier_free(barrier_t* bar);
    int barrier_sync(barrier_t* bar, int (doAction)(void*), void* action_arg);
```

* Runners functions :
```C
    void *runner_handler(void *num);
    int print_barrier(void* run);
```

### 3. Changes
* Set up the environment
* Implement the barrier mechanism
* Implement the runners handler
* Implement runners initialisation

### 4. To Do
* implement screen messages shared library

### 5. Known issues
n/a