# Temps Reel
## Exercise 5 - Kindness
---
### 1. Intro
The aim of this assignment is to re-implement the readers/writers assignment with condition variables, but using the no-starve no-priority algorithm,
in order to test the threads kindness (courtoisie in French).
The writers will therefore have no priority over the readers at first, and the priorities will be given as a program argument.
It is based on the code of the readers/writers' code in the  [assignment 4](https://github.com/gilleshenrard/ITLG_temps-reel/tree/assignment4) 

The code for the current assignment can be found on [the GitHub repository](https://github.com/gilleshenrard/ITLG_temps-reel/tree/assignment5)

Compilation and Use :
```shell
    make
    bin/readerswriters nbthreads nbwriters maximum

    make all
    make clean
```

### 2. Current features
* Threads synchronisation functions :
```C
    //barrier synchronisation functions
    barrier_t* barrier_alloc(const uint16_t nb);
    int barrier_free(barrier_t* bar);
    int barrier_sync(barrier_t* bar, int (doAction)(void*), void* action_arg);

    //FIFO synchronisation functions
    fifo_t* fifo_alloc(const uint16_t elemsz, const uint16_t amount);
    int fifo_free(fifo_t* fifo);
    int fifo_push(fifo_t* fifo, void* elem);
    void* fifo_pop(fifo_t* fifo);

//readers-writers synchronisation functions (writers have priority)
    int lightswitch_lock(lightswitch_t* light, pthread_cond_t* cond, uint8_t* flag);
    int lightswitch_unlock(lightswitch_t* light, pthread_cond_t* cond, uint8_t* flag);
    readwrite_t* rw_alloc();
    int rw_free(readwrite_t* rw);
    int rw_read(readwrite_t* rw, int (doAction)(void*), void* action_arg);
    int rw_write(readwrite_t* rw, int (doAction)(void*), void* action_arg);

    //readers-writers synchronisation functions (writers don't starve)
    readwrite_ns_t* rwnostarve_alloc();
    int rwnostarve_free(readwrite_ns_t* rw);
    int rwnostarve_read(readwrite_ns_t* rw, int (doAction)(void*), void* action_arg);
    int rwnostarve_write(readwrite_ns_t* rw, int (doAction)(void*), void* action_arg);
```

### 3. Changes
* The readers and writers now wait between 300 ms and 1s
* The no-starve algorithms have been implemented

### 4. To Do
* implement the thread priority assignment as a program argument 

### 5. Known issues
* Even withtout any priority given, the writers still seem to have priority over the readers