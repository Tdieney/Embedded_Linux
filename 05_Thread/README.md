## 1. Introduction
Threads are execution units within a process that enable concurrent task execution. Similar to processes, threads allow multi-tasking but with lower overhead in memory and CPU usage. In a multi-core system, multiple threads can run in parallel, improving performance. If one thread is blocked, others can continue executing independently.

---
## 2. Thread Operation Principles
- **Thread Creation:** A program (`process`) starts with a single thread (`main thread`). Additional threads can be created using `pthread_create()`.
- **Multi-core Processing:** Multiple threads can run simultaneously on multi-core systems.
- **Context Switching:** The CPU switches between threads, allowing multiple tasks to proceed concurrently. This involves saving and restoring thread states, leading to some overhead.
- **Memory Sharing:** Threads share the same memory space within a process, making inter-thread communication more efficient than inter-process communication.
- **Thread Stack Segments:** Each thread has its own stack, where function calls, local variables, and return addresses are stored.

### 2.1 Thread Stack Allocation in Embedded Linux
<p align="center">
    <img src="https://github.com/user-attachments/assets/61134e16-3abd-47a6-a8de-6ae78aa9d923" width="500">
</p>

- In **Embedded Linux**, when a thread is created using `pthread_create()`, its stack is **not allocated in the heap or the process’s main stack**.
- **Default Stack Behavior:**
  - The Linux kernel typically allocates the stack using `mmap()` with `MAP_PRIVATE | MAP_ANONYMOUS`.
  - Each thread gets a separate stack in the process’s virtual address space.
  - The stack location is independent of the main thread’s stack and the heap.
  - The default stack size is system-dependent and can be modified using `pthread_attr_setstacksize()`.
- **Explicit Stack Allocation:**
  - Developers can control stack memory using `pthread_attr_setstack()`, specifying both the location and size.
  ```c
  pthread_attr_t attr;
  void *stack = mmap(NULL, 1024 * 1024, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
  pthread_attr_init(&attr);
  pthread_attr_setstack(&attr, stack, 1024 * 1024);
  pthread_create(&thread, &attr, thread_function, NULL);
  pthread_attr_destroy(&attr);
  ```

---
## 3. Comparison: Process vs. Thread
| Criteria  | Process | Thread |
|-----------|---------|--------|
| Memory Space | Separate for each process | Shared within a process |
| Data Sharing | Requires IPC (Inter-Process Communication) | Direct access to shared memory |
| Creation Speed | Slower (new process requires memory allocation) | Faster (within same process) |
| Context Switching | Slower (requires more system resources) | Faster |
| Stability | One process crashing does not affect others | One thread crashing can terminate the whole process |

**Example:** A network server can handle multiple clients by either creating a new process using `fork()` or using multiple threads to process data more efficiently.
**In context swithching:** 
- **[Thread Switching](https://www.geeksforgeeks.org/threads-and-its-types-in-operating-system/) :**  
    - Thread switching is a type of context switching from one thread to another thread in the same process.
    - Thread switching is very efficient and much cheaper because it involves switching out only identities and resources such as the program counter, registers and stack pointers.
    - The cost of thread-to-thread switching is about the same as the cost of entering and exiting the kernel.
- **[Process Switching](https://www.geeksforgeeks.org/introduction-of-process-management/):**
    - Process switching is a type of [context switching](https://practice.geeksforgeeks.org/problems/what-is-context-switching) where we switch one process with another process.
    - It involves switching of all the process resources with those needed by a new process.
    - This means switching the memory address space. This includes memory addresses, page tables, and kernel resources, caches in the processor.

---
## 4. Working with Threads

### 4.1 Creating a Thread
Just like a process is identified by a **process ID**, a thread within a process is identified by a **thread ID**.

Important distinctions:
- The **process ID is unique system-wide**, whereas the **thread ID is unique only within a process**.
- The process ID is an integer, while the thread ID is not necessarily an integer; it can be a **structure**.
- The process ID can be easily printed, but thread ID retrieval requires specific functions.
- The **thread ID is represented by `pthread_t`** in POSIX systems.

Since thread IDs are often structures, comparing them requires a function rather than a direct equality check. POSIX provides two functions for this:
- `pthread_self()`: Retrieves the calling thread's ID.
- `pthread_equal()`: Compares two thread IDs.

To create a thread in C, use `pthread_create()`:
```c
#include <pthread.h>
#include <stdio.h>
void *thread_function(void *arg) {
    pthread_t tid = pthread_self();
    printf("Thread ID: %lu\n", (unsigned long)tid);
    return NULL;
}
int main() {
    pthread_t thread1, thread2;
    pthread_create(&thread1, NULL, thread_function, NULL);
    pthread_create(&thread2, NULL, thread_function, NULL);
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    return 0;
}
```
`int pthread_create(pthread_t *threadID, const pthread_attr_t *attr, void *(*start)(void *), void *arg);`
- **First argument:** Once the process is successfully called, the first argument will hold the thread ID of the newly created thread.
- b**Second argument:** This value is usually set to NULL.
- **Third argument:** This is a function pointer. Each thread runs a separate function, and the address of this function is passed in the third argument so that Linux knows where the thread starts executing.
- **Fourth argument:** The `arg` parameter is of type `void`, which allows passing any data type to the thread's processing function. Alternatively, this value can be NULL if no argument needs to be passed. This will be more clearly demonstrated in the example below.

### 4.2 Terminating a Thread
- Threads can be terminated using `pthread_exit()`:
  ```c
  pthread_exit(NULL);
  ```
`void pthread_exit(void *retval);` We can see that this function need 1 argument which is a return value of the thread called this function. The return value may be received by the parent thread when this thread is end or can be received by others thread via `pthread_join()`.

---
## 5. Thread Management

### 5.1 Joinable vs. Detached Threads
- **Joinable Thread:**
	To retrieve the exit value of another thread, we use `pthread_join()`.  
	This operation is called **joining**.
	`int pthread_join(pthread_t thread, void **retval);`
	- `pthread_join()` **blocks** until the specified thread (given in the `thread` argument) terminates.
	- If the thread has already finished, `pthread_join()` returns immediately.
	When a thread terminates, it behaves similarly to a **zombie process**.
	- If the number of zombie threads increases, at some point, it may become impossible to create new threads.
	- `pthread_join()` plays a similar role to `waitpid()` in handling these terminated threads.

	**Difference between `pthread_join()` and `waitpid()`**

	- `pthread_join()` is used for threads, whereas `waitpid()` is used for processes.
		- `thread`: The Thread ID of a specific thread.
		- `**retval`: If `retval` is not NULL, it will store the return value of `pthread_exit()`.
		- Return Value: `0` on success, and  `<0` on failure.
- **Detached Thread:**
	By default, a thread is **joinable**, meaning that when it terminates, another thread can retrieve its return value using `pthread_join()`.
	However, in many cases, we do not need to track the thread's termination state—we simply want the system to automatically clean up and remove the thread.
	In such cases, we can set the thread to a **detached state** using `pthread_detach()`.
	`int pthread_detach(pthread_t thread);`
	- Once a thread is detached, we **cannot** use `pthread_join()` to retrieve its termination status.
	- A **detached thread cannot be rejoined** or converted back to a joinable state.
	- Arguments:
		- `thread`: The Thread ID of a specific thread.
		- Return Value:  `0` on success, and `<0` on failure.

### 5.2 Thread Equal
- `pthread_equal()` is used to compare two thread IDs to check if they refer to the same thread.

---
## 6. Thread Synchronization

### 6.1 Synchronization Issues
- **Race Condition:** Occurs when multiple threads access shared resources without proper synchronization (atomic and nonatomic).
- **Deadlock:** Occurs when multiple threads wait for each other to release resources, causing indefinite blocking.
- **Critical Section:** A section of code that must be executed by only one thread at a time to prevent race conditions.

### 6.2 Mutex (Mutual Exclusion)
A mutex prevents race conditions by ensuring only one thread accesses a shared resource at a time.
The implementation of a mutex generally follows below steps:
- **Initialize the mutex**
-  **Lock the mutex** before accessing shared resources (to enter the critical section).
-  **Access the shared resources** safely.
-  **Unlock/Release the mutex** after finishing the critical section.
- (Optional) If mutex is not used anymore we can call `pthread_mutex_destroy()`. If mutex is statically initialized, we don't have to call the function.
  ```c
  pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&mutex);
  // Critical section
  pthread_mutex_unlock(&mutex);
  ```
- **Allocating a Mutex:**
  ```c
  int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *attr);
  ```
- **Mutex Deadlocks:** Can occur if multiple threads lock resources in an inconsistent order. Proper handling is required to avoid deadlocks.
- **Locking and Unlocking a Mutex:** Ensures exclusive access to shared resources.

Among the cases where we must use `pthread_mutex_init()` rather than a static  
initializer are the following:
➡️ **The mutex is dynamically allocated on the heap**
	- If you allocate a mutex dynamically using `malloc()` or if it is part of a struct that is dynamically allocated, you **cannot** use `PTHREAD_MUTEX_INITIALIZER`.
	- **Reason:** `PTHREAD_MUTEX_INITIALIZER` only works with **global** or **static** variables.
	- **Solution:** You **must** call `pthread_mutex_init()` to initialize the mutex after allocating memory.
📌 **Example**:
```c
pthread_mutex_t *mutex = malloc(sizeof(pthread_mutex_t));
pthread_mutex_init(mutex, NULL);  // Must use this function
```

➡️ **The mutex is an automatic (local) variable on the stack**
	- If you declare a mutex **inside a function** (i.e., on the stack), you **cannot** use `PTHREAD_MUTEX_INITIALIZER`.
	- **Reason:** `PTHREAD_MUTEX_INITIALIZER` only works with **global** or **static** variables.
	- **Solution:** Use `pthread_mutex_init()` to initialize the mutex.
📌 **Example**:
```c
void myFunction() {
    pthread_mutex_t mutex;  // Local variable on stack
    pthread_mutex_init(&mutex, NULL);  // Must call init before using
}
```

➡️ **You want to initialize a statically allocated mutex with non-default attributes**
	- `PTHREAD_MUTEX_INITIALIZER` can only initialize a mutex with **default attributes**.
	- If you need custom attributes (e.g., **recursive mutex** or **error-checking mutex**), you **must** use `pthread_mutex_init()` with a `pthread_mutexattr_t` object.
📌 **Example** (creating a **recursive mutex**):
```c
pthread_mutex_t mutex;
pthread_mutexattr_t attr;

pthread_mutexattr_init(&attr);
pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
pthread_mutex_init(&mutex, &attr);

pthread_mutexattr_destroy(&attr);  // Free attribute object
```

### 6.3 Condition Variables
A **Condition Variable** is a synchronization mechanism in multithreading that allows threads to communicate with each other based on specific conditions. Condition Variables are often used in combination with **Mutexes** to control access to shared resources.
#### Why?
- In a multithreading application, when a thread needs to wait for a certain condition (e.g., a resource becoming available), it can block without consuming CPU resources.
- Condition Variables allow a thread to **wait** until it receives a **signal** from another thread indicating that the condition has been met.
#### Condition Variables and Mutexes
- **Mutex (Mutual Exclusion)** ensures that only one thread can access a shared resource at a time.
- **Condition Variables** notify waiting threads about changes in shared resources and allow blocked threads to resume execution.
#### How it works:
- A thread **waits** on a Condition Variable if the required condition is not met.
- Another thread modifies the shared resource and **signals** the waiting thread to continue execution.
#### Allocating and Initializing Condition Variables
In Linux, a Condition Variable is defined using `pthread_cond_t`. Before using it, it must be initialized in one of two ways:
- **Static allocation:**
    ```c
    pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    ```
- **Dynamic allocation:**
    ```c
    int pthread_cond_init(pthread_cond_t *cond, const pthread_condattr_t *attr);
    ```

#### Destroying a Condition Variable
- When no longer needed, a dynamically allocated Condition Variable should be destroyed using:
    ```c
    pthread_cond_destroy(&cond);
    ```
- If using static initialization (`PTHREAD_COND_INITIALIZER`), calling `pthread_cond_destroy()` is not required.
#### Using Condition Variables for Signaling and Waiting
Condition Variables support two main operations:
- **Signal:** Notifies a waiting thread that the condition has been met.
- **Wait:** Blocks the thread until the condition is satisfied.
##### Example:
```c
#include <pthread.h>
#include <stdio.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
int shared_data = 0;

void *producer(void *arg) {
    pthread_mutex_lock(&mutex);
    shared_data = 1; // Update shared data
    pthread_cond_signal(&cond); // Notify consumer
    pthread_mutex_unlock(&mutex);
    return NULL;
}

void *consumer(void *arg) {
    pthread_mutex_lock(&mutex);
    while (shared_data == 0) {
        pthread_cond_wait(&cond, &mutex); // Wait for signal from producer
    }
    printf("Consumer: Received data!\n");
    pthread_mutex_unlock(&mutex);
    return NULL;
}

int main() {
    pthread_t t1, t2;
    pthread_create(&t1, NULL, consumer, NULL);
    pthread_create(&t2, NULL, producer, NULL);
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);
    return 0;
}
```

##### Explanation of the Code
- The `producer()` function updates `shared_data` and sends a signal using `pthread_cond_signal()`.
- The `consumer()` function checks `shared_data`, and if it's not ready, it calls `pthread_cond_wait()`.
- When `pthread_cond_signal()` is called, the blocked `pthread_cond_wait()` exits, allowing the consumer to continue execution.

---
## 7. Thread Termination
- The thread terminates normally.
- The thread ends when calling the `pthread_exit()` function.
- The thread is canceled when calling the `pthread_cancel()` function.
- If any thread calls the `exit()` function or the main thread terminates, all remaining threads will immediately terminate.

**`pthread_exit(void *retval);`**
- A thread can terminate by calling `pthread_exit()`.
- **Arguments:**
       - `*retval`: Specifies the return value of the thread, which can be retrieved by another thread using `pthread_join()`.
- **Return Value:** `0` on success, and `<0` on failure.

 **`pthread_cancel(pthread_t thread);`**
- `pthread_cancel()` sends a termination request to a specific thread.
- **Arguments:**
       - `thread`: The thread ID of a specific thread.
- **Return Value:** `0` on success, and  `<0` on failure.

---
## 8. Summary
- **Threads enable efficient multi-tasking** within a process but require proper management.
- **Thread synchronization techniques (Mutex, Condition Variables)** help prevent race conditions and deadlocks.
- **Understanding Joinable vs. Detached Threads** is essential for designing robust multi-threaded applications.
- **Mutex Deadlocks and Race Conditions** should be carefully handled to ensure program stability.
- **Proper termination of threads** prevents memory leaks and resource contention issues.
- **Thread stack allocation in Embedded Linux** typically occurs in the heap but can be explicitly controlled using `pthread_attr_setstacksize()` or `pthread_attr_setstack()`.