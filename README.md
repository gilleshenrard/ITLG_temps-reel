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

Use :
```shell
    ./runners nb_laps
```

### 2. Current features
The initial setup (program, directories tree, makefiles, libraries) is done.

### 3. Changes since latest release
n/a

### 4. To Do
* implement the Barrier function
* implement the runners handler function
* implement error management code 

### 5. Known issues
n/a