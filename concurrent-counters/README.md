# C++ Multi-threading Synchronization Examples

This repository contains several example programs that demonstrate different synchronization techniques in C++ using various threading libraries and mechanisms. Each set of examples showcases three common synchronization methods:

- **Mutex Try-Lock**  
- **Compare-and-Swap (CAS)**
- **Fetch-and-add**

For each technique, implementations using standard atomic operations, mutex-based locks, OpenMP, and Pthreads are provided.

---

## Prerequisites

Before compiling and running these examples, ensure that your environment has the following:

- A C++ compiler supporting C++11 or later (e.g., `g++`)
- OpenMP support (for the OpenMP variants)
- Pthreads library (for the pthreads variants)
- A terminal or command prompt environment (on Windows, you might use a shell like Git Bash or use the Command Prompt with appropriate adjustments)

---

## Compilation Instructions

Navigate to the project directory in your terminal, then compile each example using the commands below.

### Mutex Try-Lock Examples

Compile the Mutex try-lock examples:

```bash
g++ -o Mutex_try_lock Mutex_try_lock.cpp
g++ -fopenmp -o Mutex_try_lock_openMP Mutex_try_lock_openMP.cpp
g++ -pthread -o Mutex_try_lock_pthreads Mutex_try_lock_pthreads.cpp
```

### Compare-and-Swap Examples

Compile the Compare-and-Swap examples:

```bash
g++ -o Compare-and-Swap_atomic Compare-and-Swap_atomic.cpp
g++ -o Compare-and-Swap_mutex Compare-and-Swap_mutex.cpp
g++ -fopenmp -o Compare-and-Swap_openMP Compare-and-Swap_openMP.cpp
g++ -pthread -o Compare-and-Swap_pthreads Compare-and-Swap_pthreads.cpp
```

### Fetch-and-add Examples

Compile the Fetch-and-add examples:

```bash
g++ -o Fetch_and_add_atomic Fetch_and_add_atomic.cpp
g++ -o Fetch_and_add_mutex Fetch_and_add_mutex.cpp
g++ -fopenmp -o Fetch_and_add_openMP Fetch_and_add_openMP.cpp
g++ -pthread -o Fetch_and_add_pthreads Fetch_and_add_pthreads.cpp
```

### Running the Examples
```bash
./Mutex_try_lock
./Mutex_try_lock_openMP
./Mutex_try_lock_pthreads

./Compare-and-Swap_atomic
./Compare-and-Swap_mutex
./Compare-and-Swap_openMP
./Compare-and-Swap_pthreads

./Fetch_and_add_atomic
./Fetch_and_add_mutex
./Fetch_and_add_openMP
./Fetch_and_add_pthreads
```