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
    bin/readerswriters nbthreads nbwriters maximum [nice_readers nice_writers]

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
    int lightswitch_lock(lightswitch_t* light, pthread_mutex_t* mutex);
    int lightswitch_unlock(lightswitch_t* light, pthread_mutex_t* mutex);
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
* The readers now wait between 100 ms and 400ms and the writers between 300ms and 1s
* The readers/writers algorithms now implement the solution offered by Laura Binacchi (mutexes instead of condition variables)
* The niceness values can now be given as facultative program argument (both 0 if not specified) 
* The lightswitches code is now also more compact
* A function generating true 32 bits random numbers has been implemented

### 4. To Do
n/a

### 5. Known issues
n/a