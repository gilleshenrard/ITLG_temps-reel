# Temps Reel
## Exercise 4 - Condition Variables
---
### 1. Intro
The aim of this assignment is to re-implement the previous exercises, by replacing semaphores by condition variables.
The previous assignments are the following :
* [Runners (Barrier problem)](https://github.com/gilleshenrard/ITLG_temps-reel/tree/assignment1)
* [Producers-Consumers](https://github.com/gilleshenrard/ITLG_temps-reel/tree/assignment2)
* [Readers-Writers](https://github.com/gilleshenrard/ITLG_temps-reel/tree/assignment3) 

The code for the current assignment can be found on [the GitHub repository](https://github.com/gilleshenrard/ITLG_temps-reel/tree/assignment4)

Use :
```shell
    bin/runners nb_runners nb_laps
    bin/prodcons sz_fifo filename
    bin/readerswriters nbthreads nbwriters maximum
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

    //readers-writers synchronisation functions
    int lightswitch_lock(lightswitch_t* light, pthread_cond_t* cond, uint8_t* flag);
    int lightswitch_unlock(lightswitch_t* light, pthread_cond_t* cond, uint8_t* flag);
    int rw_alloc(readwrite_t** rw);
    int rw_free(readwrite_t* rw);
    int rw_read(readwrite_t* rw, int (doAction)(void*), void* action_arg);
    int rw_write(readwrite_t* rw, int (doAction)(void*), void* action_arg);
```

### 3. Changes
* Replace all semaphores in the structures by condition variables
* move all unnecessart library calls from *.h files to *.c files (speed up compilation)
* readers/writers lighswitches now use pthread_mutex_trylock to make sure the mutex is locked
* writers : max value verification has been moved to the critical section (more accurate)
* renamed readwrite files and functions to rwprocess to ease up code reading

### 4. To Do
n/a

### 5. Known issues
n/a