# Temps Reel
## Exercise 2 - Producers-consumers
---
### 1. Intro
The aim of this assignment is to implement the FIFO multithreading synchronisation technique, as well as
the Producers-Consumers problem.

3 procedures are implemented as 3 threads :
- The first one reads a text file character by character and pushes each character in a FIFO queue.
- The second one pops each character from the first FIFO, and outputs it as upper cased in a second FIFO.
- The third one pops each upper case character from the second FIFO and outputs it on the screen.

The code can be found on [the GitHub repository](https://github.com/gilleshenrard/ITLG_temps-reel/tree/assignment2)

Use :
```shell
    bin/prodcons sz_fifo filename
```

### 2. Current features
* Threads synchronisation functions :
```C
    //barrier synchronisation functions
    int barrier_alloc(barrier_t** bar, const uint16_t nb);
    int barrier_free(barrier_t* bar);
    int barrier_sync(barrier_t* bar, int (doAction)(void*), void* action_arg);

    //FIFO synchronisation functions
    int fifo_alloc(fifo_t** fifo, const uint16_t elemsz, const uint16_t amount);
    int fifo_free(fifo_t* fifo);
    int fifo_push(fifo_t* fifo, void* elem);
    void* fifo_pop(fifo_t* fifo);
```

* ProdCons functions :
```C
    void *readproc_handler(void *proc);
    void *calcproc_handler(void *proc);
    void *dispproc_handler(void *proc);
```

### 3. Changes
all producer/consumer functions + FIFO functions have been implemented

### 4. To Do
* implement screen messages shared library
* move processes allocation/deallocation functions to the process library

### 5. Known issues
n/a