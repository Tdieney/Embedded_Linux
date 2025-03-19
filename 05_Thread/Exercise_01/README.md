### **Question:** How to use `pthread_create` and `pthread_join`? When does the thread end?  

### **Answer:**  
#### **`pthread_create`**
To create a new thread, we use `pthread_create()`
`int pthread_create(pthread_t *threadID, const pthread_attr_t *attr, void *(*start)(void *), void *arg);`
- **`*threadID`:** Once the process is successfully called, the first argument will hold the thread ID of the newly created thread.
- **`pthread_attr_t`:** This value is usually set to NULL.
- **`*(*start)(void *)`:** This is a function pointer. Each thread runs a separate function, and the address of this function is passed in the third argument so that Linux knows where the thread starts executing.
- **`*arg`:** The `arg` parameter is of type `void`, which allows passing any data type to the thread's processing function. Alternatively, this value can be NULL if no argument needs to be passed. This will be more clearly demonstrated in the example below.

#### **`pthread_join`**
To retrieve the exit value of another thread, we use `pthread_join()`.
- `pthread_join()` **blocks** until the specified thread (given in the `thread` argument) terminates.
- If the thread has already finished, `pthread_join()` returns immediately.

`int pthread_join(pthread_t thread, void **retval);`
- **`thread`:**  is the thread identifier

- **`retval`:**  contains a pointer to the status argument passed by the ending thread as part of pthread_exit(). If the ending thread terminated with a return, `retval` contains a pointer to the return value. If the thread was canceled, status can be set to -1.

When a thread terminates, it behaves similarly to a **zombie process**.
- If the number of zombie threads increases, at some point, it may become impossible to create new threads.
- `pthread_join()` plays a similar role to `waitpid()` in handling these terminated threads.


### **When does a thread end?**
4 main ways a thread can terminate:
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

⏳ Note: If a thread is terminated without `pthread_join()` or `pthread_detached()`, it can cause memory leaks if resources are not properly freed.


