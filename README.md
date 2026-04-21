# LAB 3 - OS - Synchronization

## Overview
This lab focuses on synchronization mechanisms in operating systems to solve race condition problems in multi-threaded environments. It covers mutex locks, semaphores, conditional variables, and various locking techniques.

---

## Practices

### Practice 3.1: Shared Buffer Problem (p1mutex)
**Location:** `labSync-student/p1mutex/`

**Description:** Demonstrates race condition when multiple threads access shared memory without synchronization. Two threads increment a shared counter, resulting in incorrect final values due to non-atomic operations.

**How to Run:**
```bash
cd labSync-student/p1mutex
gcc -pthread -o shrdmem shrdmem.c
./shrdmem
```

**Expected Issue:** The final count will be less than 2,000,000,000 due to race conditions.

**Solution:** Use `pthread_mutex_lock()` and `pthread_mutex_unlock()` to protect the critical section.

---

### Practice 3.2: Bounded Buffer Problem (p2pc)
**Location:** `labSync-student/p2pc/`

**Description:** Producer-consumer problem where producers add items to a bounded buffer and consumers retrieve them. Without proper synchronization, data can be overwritten or garbage values retrieved.

**How to Run:**
```bash
cd labSync-student/p2pc
gcc -pthread -o pc pc.c
./pc
```

**Expected Issue:** Consumers may read duplicate or uninitialized values.

**Solution:** Use semaphores (`sem_wait()` and `sem_post()`) to manage empty and filled slots.

---

## Exercises

### Exercise 1: Sequence Lock API (ex1seqlock)
**Location:** `labSync-student/ex1seqlock/`

**Description:** Implement a sequence lock mechanism that gives priority to writers over readers, preventing writer starvation. Uses a sequence number to detect concurrent writes.

**How to Run:**
```bash
cd labSync-student/ex1seqlock
gcc -pthread -o seqlock seqlock.c
./seqlock
```

**Key Concepts:**
- Odd sequence number = write in progress
- Even sequence number = safe to read
- Writers can interrupt readers without blocking

---

### Exercise 2: Aggregated Sum (ex2aggsum)
**Location:** `labSync-student/ex2aggsum/`

**Description:** Calculate the sum of an integer array using multiple threads in parallel. Each thread computes a partial sum for its assigned range, then safely adds it to a global sum buffer using mutex synchronization.

**How to Run:**
```bash
cd labSync-student/ex2aggsum
make
./aggsum <array_size> <num_threads> [seed]
```

**Example:**
```bash
./aggsum 100 4 1024
./aggsum 1000 8 2048
```

**Implementation Details:**
- Splits array across threads
- Each thread calculates local sum
- Mutex protects global sum update
- Child process validates with sequential sum

---

### Exercise 3: Interruptable System Logger (ex3logbuf)
**Location:** `labSync-student/ex3logbuf/`

**Description:** Implement a thread-safe logger with bounded buffer. Multiple threads write logs asynchronously while a periodic flush operation clears the buffer. Uses semaphores for producer-consumer synchronization.

**How to Run:**
```bash
cd labSync-student/ex3logbuf
gcc -pthread -o logbuf logbuf.c
./logbuf
```

**Implementation Details:**
- Buffer size: 6 slots
- Writers block when buffer is full
- Flush operation runs every 500ms
- Semaphores: `empty_slots` and `filled_slots`

---

### Exercise 4: Wait and Notify (ex4waitnotify)
**Location:** `labSync-student-2411141/ex4waitnotify/`

**Description:** Producer-consumer pattern using condition variables. Writers add items to buffer and notify readers. Readers wait for items and notify writers when space is available.

**How to Run:**
```bash
cd labSync-student-2411141/ex4waitnotify
gcc -pthread -o waitnotify waitnotify.c
./waitnotify
```

**Implementation Details:**
- Two condition variables: `cond_not_full` and `cond_not_empty`
- Writers signal readers when adding items
- Readers signal writers when removing items
- Prevents busy-waiting with efficient blocking

---

### Exercise 5: Periodic Detection with Recovery (ex5periodic)
**Location:** `labSync-student-2411141/ex5periodic/`

**Description:** Implement a periodic monitoring system that checks for unsafe states and performs automatic recovery. A detector thread runs every 5 seconds to check system health.

**How to Run:**
```bash
cd labSync-student-2411141/ex5periodic
gcc -pthread -o periodic periodic.c
./periodic
```

**Implementation Details:**
- Detector checks every 5 seconds
- Monitors resource values (0-100 range)
- Tracks error count (max 3 errors)
- Automatic recovery when thresholds exceeded
- Worker threads simulate operations

---

### Exercise 6: Asynchronous Resource Requests (ex6async)
**Location:** `labSync-student-2411141/ex6async/`

**Description:** Implement non-blocking resource allocation with callback mechanism. Processes request resources asynchronously and are notified via callback when resources become available.

**How to Run:**
```bash
cd labSync-student-2411141/ex6async
gcc -pthread -o async_resource async_resource.c
./async_resource
```

**Implementation Details:**
- Total resources: 10
- Processes request varying amounts (2-4 resources)
- Callback invoked when resources allocated
- Condition variable broadcasts when resources released
- Fair allocation with no busy-waiting

---

## Synchronization Mechanisms Used

### Mutex Lock
- **Purpose:** Protect critical sections from concurrent access
- **Functions:** `pthread_mutex_lock()`, `pthread_mutex_unlock()`
- **Used in:** Exercise 2, 4, 5, 6

### Semaphore
- **Purpose:** Manage bounded resources and ordering
- **Functions:** `sem_wait()`, `sem_post()`
- **Used in:** Practice 3.2, Exercise 3

### Condition Variables
- **Purpose:** Efficient waiting for conditions to become true
- **Functions:** `pthread_cond_wait()`, `pthread_cond_signal()`, `pthread_cond_broadcast()`
- **Used in:** Exercise 4, 6

### Spinlock
- **Purpose:** Busy-wait locking for short critical sections
- **Functions:** `pthread_spin_lock()`, `pthread_spin_unlock()`
- **Used in:** API examples

### Sequence Lock
- **Purpose:** Reader-writer lock with writer priority
- **Used in:** Exercise 1

---

## Compilation Notes

### General Compilation
Most programs can be compiled with:
```bash
gcc -pthread -o <output> <source.c>
```

### Using Makefile
For Exercise 2 (aggsum), use the provided Makefile:
```bash
make          # Compile
make clean    # Clean build files
```

### Common Flags
- `-pthread`: Enable POSIX thread support
- `-g`: Include debug symbols
- `-o`: Specify output file name

---

## Testing Tips

1. **Run multiple times** to observe race conditions
2. **Vary parameters** (thread count, array size, etc.)
3. **Check output** for consistency between parallel and sequential results
4. **Monitor timing** to see synchronization overhead
5. **Use different seeds** for random data generation

---

## Common Issues

### Compilation Errors
- Missing `-pthread` flag
- Undefined references to pthread functions
- Solution: Always use `-pthread` flag

### Runtime Issues
- Deadlocks: Check lock ordering
- Race conditions: Ensure all shared data is protected
- Segmentation faults: Check array bounds and pointer usage

### WSL/Linux Environment
If on Windows, use WSL:
```bash
wsl gcc -pthread -o program program.c
wsl ./program
```

