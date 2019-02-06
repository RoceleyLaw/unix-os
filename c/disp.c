/* disp.c : dispatcher
 */
#include <xeroskernel.h>
#include <stdarg.h>
extern pcb_t *stopped_queue_tail;
extern pcb_t *ready_queue_tail;
extern pcb_t *blocked_queue_tail;
extern pcb_t *running_process;
// Pass parameter from context switcher in
// arg[0]: func ptr
// arg[1]: stack size
extern int args[2];
void dispInit(void) {
    initpcbtable();
}

/* removes the next process from the ready queue 
   and returns an index or a pointer to its process control block. */
pcb_t *next(void) {
    // TODO: remove the process, not just dequeue the pcb.
    pcb_t *next_pcb = dequeuepcb(READY);
    return next_pcb;
}

/*  takes an index or pointer to a process control block 
    and adds it to the ready queue. */
void ready(pcb_t* pcb_ptr) {
    pcb_ptr -> state = READY;
    enqueuepcb(READY, pcb_ptr);
}

void cleanup(pcb_t *p) {
    // kprintf("\n Cleanup is called \n\n\n");
    // for(;;);
	//set the process state to STOPPED
	p -> state = STOPPED;
	//free the allocated space for that process
	kfree(p -> buff);
	//add the process to STOPPED queue
	enqueuepcb(STOPPED, p);
}

/* An infinite loop that processes system calls,
    schedules the next process, 
    and then calls the context switcher to switch into the next scheduled process.
    three different events corresponding to the system calls 
    - syscreate(), 
    - sysstop(), 
    - sysyield(): On a sysyield call, the dispatcher places the current process at the end of the ready
                  queue, and removes the next ready process from queue and makes it the current process. 
After completing the requesting operation, the dispatcher calls the context switcher to switch into the current process.*/
void dispatch(void) {
    // process = next();
    // for( ;; ) 
    // request = contextswitch( process );
    // switch( request ) {
    //     case( CREATE ): create(new process) + place into the ready queue; break;
    //     case( YIELD ): ready( process ); process = next(); break;
    //     case( STOP ): cleanup( process ); process = next(); free the stack space and PCB; break; }
    pcb_t* process = next();
    // kprintf("WHAT's our next process??");

    while (1) {
        // todo: when would contextswitcher returns? 
        // dispatcher is working for the kernel, which means when we call dispatch,
        // we are at kernel mode
        int request = contextswitch(process);
        switch (request)
        {
            case CREATE:
            {   // kprintf("\n GET A CREATE req");
        		// Create a new process
	        	void (*func)(void);
	        	func = args[0];
	        	for (int i = 0; i < 4000000; i++);
	        	// kprintf("\n disp : function address is %d", args[1]);
	        	int stack = args[1];
                // kprintf("\n va_arg: %d, %d", func, args[1]);
	        	create(func, stack);
	        	break;
	       	}
            case YIELD:
            {
                // Yeild the process. process here is a pcb ptr. 
                // We want yield the currently running process
                ready(process);
                process = next();
                break;
            }
            case STOP:
            {
                cleanup(process); 
                process = next();
                break;
            }
            default:
            {
                // kprintf("\n Error: unknown request received. Please check your disp.c \n");
                break;
            }
        }
    }
}