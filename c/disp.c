/* disp.c : dispatcher
 */
#include <xeroskernel.h>
extern pcb_t *stopped_queue_tail;
extern pcb_t *ready_queue_tail;
extern pcb_t *blocked_queue_tail;
extern pcb_t *running_process;

extern void dispInit() {
    initpcbtable();
}

/* removes the next process from the ready queue 
   and returns an index or a pointer to its process control block. */
pcb_t *next() {
    // TODO: remove the process, not just dequeue the pcb.
    pcb_t *next_pcb = dequeuepcb(READY);
    return next_pcb;
}

/*  takes an index or pointer to a process control block 
    and adds it to the ready queue. */
void ready(pcb_t* pcb_ptr) {
    enqueuepcb(READY, pcb_ptr);
}

void cleanup(pcb_t* pcb_ptr) {
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
extern void dispatch() {
    // process = next();
    // for( ;; ) 
    // request = contextswitch( process );
    // switch( request ) {
    //     case( CREATE ): create(new process) + place into the ready queue; break;
    //     case( YIELD ): ready( process ); process = next(); break;
    //     case( STOP ): cleanup( process ); process = next(); free the stack space and PCB; break; }
    pcb_t* process = next();
    while (1) {
        // todo: when would contextswitcher returns? 
        // dispatcher is working for the kernel, which means when we call dispatch,
        // we are at kernel mode
        int request = contextswitch(process);
        // switch (request)
        // {
        //     case CREATE:
        //     // TODO: not correct here, create() in create.c is not returning any ptr
        //         // pcb_t* new_proc = create(NULL, 0);
        //         // enqueuepcb(READY, new_proc);
        //         break;
        //     case YIELD:
        //         // TODO: think of how to get current proc info
        //         ready(process);
        //         process = next();
        //         break;
        //     case STOP:
        //         cleanup(process); 
        //         process = next();
        //         break;
        //     default:
        //         break;
        // }
    }

}