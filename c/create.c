/* create.c : create a process
 */

#include <xeroskernel.h>

/* Your code goes here. */
// the head ptr of the queue linkedlist
// initially, every pcb would be in the stopped queue
extern pcb_t *stopped_queue;
extern pcb_t *ready_queue;

/* The kernel uses this function to create new processes 
    and add them to the ready queue.  
    Parameters: 
    - a reference (function pointer) to the start of the process code 
    - an integer denoting the amount of stack to allocate for the process. 
    Return:
    - 1 on success
    - 0 on failure.*/
extern int create(void (*func)(void), int stack) {
    // if (!(unsigned long)(*func)) {
    //     kprintf("Error: invalid function address");
    //     return 0;
    // }

    // if (stack <= 0) {
    //     kprintf("Warning: invalid stack size");
    //     return 0;
    // }

    // initialize the process control block
    
    pcb_t* unused = dequeuepcb(STOPPED);
    unused = dequeuepcb(STOPPED);

    return 0;
}