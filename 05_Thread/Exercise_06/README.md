### **Question:** Why is a mutex necessary in this exercise? What happens if we remove the mutex?  

### **Answer:**  
A **mutex (Mutual Exclusion)** prevents **race conditions** when multiple tasks modify a shared global variable (in this exercise is "counter" variable). Without it, tasks may access the variable simultaneously, causing **data corruption, unpredictable results, or crashes**.  

#### **Why do we need a mutex?**  
- Ensures data integrity by allowing only one task to modify the variable at a time.  
- Prevents race conditions where tasks overwrite each other's updates.  
- Maintains predictable execution.  

#### **What happens if we remove the mutex?**
- Unpredictable behaviour and data corruption.
- Potential crashes.

