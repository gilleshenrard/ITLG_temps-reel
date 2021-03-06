# Temps Reel
## Exercise 3 - Readers-Writers
---
### 1. Intro
The aim of this assignment is to implement the Readers-Writers problem.

A certain amount of threads are created. Amongst them are writers and readers.
The amount of threads and the amount of writers are defined as a program argument.

All the threads have a shared data variable, which is a uint16_t.

The writers have a higher priority than the readers and simply increment the data value up until a maximum.
The readers read the data value and display it on the screen.

The maximum value is also defined as a program argument.

Use :
```shell
    bin/readerswriters nbthreads nbwriters maximum
```

### 2. Current features
* Threads synchronisation functions :
```C
    //lightswitch functions ensuring either the readers or the writers
    //  will wait for the current others to finish their tasks
    int lightswitch_lock(lightswitch_t* light, sem_t* sem);
    int lightswitch_unlock(lightswitch_t* light, sem_t* sem);

    //readers-writers synchronisation functions
    int rw_alloc(readwrite_t** rw);
    int rw_free(readwrite_t* rw);
    int rw_read(readwrite_t* rw, int (doAction)(void*), void* action_arg);
    int rw_write(readwrite_t* rw, int (doAction)(void*), void* action_arg);
```
* Readers/Writers processes functions :
```C
    //structures allocation/deallocation
    int readwrite_alloc(thrw_t** reader, readwrite_t* rw, const uint16_t thnum, uint16_t* data, const uint16_t max);
    int readwrite_assign(thrw_t* reader, readwrite_t* rw, const uint16_t thnum, uint16_t* data, const uint16_t max);
    int readwrite_free(thrw_t* reader);

    //Readers business logic functions
    void *reader_handler(void *reader);
    int displayData(void* reader);

    //Readers business logic functions
    void *writer_handler(void *writer);
    int updateData(void* writer);
```

### 3. Currently implemented in the final assignment
* threads and structures allocation/deallocation
* threads launching
* threads synchronisation (final join)
* error management

### 4. Changes since latest release
n/a

### 5. To Do
n/a

### 6. Known issues
n/a