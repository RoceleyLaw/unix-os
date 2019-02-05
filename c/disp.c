/* disp.c : dispatcher
 */

#include <xeroskernel.h>

/* Your code goes here */
extern void dispatch() {
    // process = next();
    // for( ;; ) 
    // request = contextswitch( process );
    // switch( request ) {
    //     case( CREATE ): create(new process) + place into the ready queue; break;
    //     case( YIELD ): ready( process ); process = next() + remove the process from ready queue; break;
    //     case( STOP ): cleanup( process ); process = next(); free the stack space and PCB; break; }
    pcb_t* new_proc = &pcb_table[0];
    enqueuepcb(READY,&pcb_table[0]);
    pcb_t* process = next();
    kprintf("NEXT PROCESS: %d", (int)process);
}

/* removes the next process from the ready queue 
   and returns an index or a pointer to its process control block. */
pcb_t *next() {
    // TODO: remove the process, not just dequeue the pcb.
    return dequeuepcb(READY);
}

/*  takes an index or pointer to a process control block 
    and adds it to the ready queue. */
void ready(pcb_t* pcb_ptr) {
    enqueuepcb(READY, pcb_ptr);
}

void cleanup(pcb_t* pcb_ptr) {
}