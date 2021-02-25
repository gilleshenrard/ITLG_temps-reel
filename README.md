# Temps Reel
## Exercise 2 - Producers-consumers
---
### 1. Intro
The aim of this assignment is to implement the FIFO multithreading synchronisation technique, as well as
the Producers-Consumers problem.

The code can be found on [the GitHub repository](https://github.com/gilleshenrard/ITLG_temps-reel/tree/assignment2)

Use :
```shell
    bin/prodcons
```

### 2. Current features
* Threads synchronisation functions :
```C
    int barrier_alloc(barrier_t** bar, const uint16_t nb);
    int barrier_free(barrier_t* bar);
    int barrier_sync(barrier_t* bar, int (doAction)(void*), void* action_arg);
```

* ProdCons functions :
```C
```

### 3. Changes
n/a

### 4. To Do
* implement screen messages shared library

### 5. Known issues
n/a