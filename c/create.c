/* create.c : create a process
 */

#include <xeroskernel.h>

/* Your code goes here. */
// the head ptr of the queue linkedlist
// initially, every pcb would be in the stopped queue
extern pcb_t *stopped_queue;
extern pcb_t *ready_queue;
int SAFETY_MARGIN = 16;
/* The kernel uses this function to create new processes 
    and add them to the ready queue.  
    Parameters: 
    - a reference (function pointer) to the start of the process code 
    - an integer denoting the amount of stack to allocate for the process. 
    Return:
    - 1 on success
    - 0 on failure.*/
extern int create(void (*func)(void), int stack) {
    if (!(unsigned long)(*func)) {
        kprintf("Error: invalid function address");
        return 0;
    }

    if (stack <= 0) {
        kprintf("Warning: invalid stack size");
        return 0;
    }

    // initialize the process control block
    pcb_t *new_pcb = dequeuepcb(STOPPED);
    //malloc new memory for our new process
    void* new_memory = kmalloc((size_t) stack);
    //set the allocated space and the size of the space to pcb
    new_pcb -> buff = new_memory;

    unsigned long amnt = ((stack) / 16 + ((stack % 16)? 1:0))*16;

    kprintf("stack: %d, amount: %d", stack, amnt);

    // Point to end of the allocated memory chunk 
    // Subtract a safety margin and size of context frame 
    // Stack pointer starts here 
    void* tempEsp = new_memory + amnt - SAFETY_MARGIN - sizeof(context_frame_t);
    new_pcb -> esp = tempEsp;
    // // kprintf("\nnew_memory: %d, tempEsp: %d", new_memory, tempEsp);

    //Initialize the context frame
    
    //TODO: !!!!!!!!!!!!!!!! (Fill in eflags!)
    kprintf("\ngetCS value: %d", getCS());
    //initialize the new context frame value
    context_frame_t* new_ctf = (context_frame_t*) tempEsp;
    new_ctf -> eflags = 0;
    new_ctf->iret_cs = getCS();
    new_ctf->iret_eip = &func;
    new_ctf->eax = 0;
    new_ctf->ecx = 0;
    new_ctf->edx = 0;
    new_ctf->ebx = 0;  
    new_ctf->esp = tempEsp;
    new_ctf->ebp = tempEsp;
    new_ctf->esi = 0; 
    new_ctf->edi = 0;
    
    // // Assign the context frame value
    // context_frame_t* ctf_bottom = (context_frame_t*)tempEsp;
    // kprintf("TEMP ESP: %d", ctf_bottom);
    // ctf_bottom -> eflags = new_ctf.eflags;
    // ctf_bottom -> iret_cs = new_ctf.iret_cs;
    // ctf_bottom -> iret_eip = new_ctf.iret_eip;
    // ctf_bottom -> eax = new_ctf.eax;
    // ctf_bottom -> ecx = new_ctf.ecx;
    // ctf_bottom -> edx = new_ctf.edx;
    // ctf_bottom -> ebx = new_ctf.ebx;
    // ctf_bottom -> esp = new_ctf.esp;
    // ctf_bottom -> ebp = new_ctf.ebp;
    // ctf_bottom -> esi = new_ctf.esi;
    // ctf_bottom -> edi = new_ctf.edi;

                 
    // *(long*)(tempEsp)   = 0;                   //edi
    // *(long*)(tempEsp+4) = 0;                   //esi
    // *(long*)(tempEsp+8) = tempEsp;             //ebp
    // *(long*)(tempEsp+12) = tempEsp;            //esp
    // *(long*)(tempEsp+16) = 0;                  //ebx
    // *(long*)(tempEsp+20) = 0;                  //edx
    // *(long*)(tempEsp+24) = 0;                  //ecx
    // *(long*)(tempEsp+28) = 0;                  //eax
    // *(long*)(tempEsp+32) = func;               //eip
    // *(long*)(tempEsp+36) = getCS();;           //cs
    // *(long*)(tempEsp+40) = 0;                  //edi
    for(int i = 0; i < 11; i++){
        kprintf("new_ctf : %d",  *(unsigned long*)(new_ctf + i*4));
    }
    for(;;);
    new_pcb -> state = READY;
    enqueuepcb(READY, new_pcb);
    return 1;
}