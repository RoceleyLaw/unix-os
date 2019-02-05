/* create.c : create a process
 */

#include <xeroskernel.h>

/* Your code goes here. */
// the head ptr of the queue linkedlist
// initially, every pcb would be in the stopped queue
extern pcb_t *stopped_queue;
extern pcb_t *ready_queue;
int SAFETY_MARGIN = 16;
extern struct memHeader {
// size - the size of the free slot + header
  unsigned long size;   // - 4 bytes
  struct memHeader *prev; // - 4 bytes
  struct memHeader *next; // - 4 bytes
  char *sanityCheck; // - 4 bytes; 0 for freed space; 1 for allocated space
  
  // Used to track the address of the start of the actual space of the Node:
  // addr + sizeof(struct memHeader) (zero sized array trick)
  unsigned char dataStart[0];
};
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
    struct memHeader* getHeader = (struct memHeader*)((unsigned long)new_memory - sizeof(struct memHeader));
    kprintf("\ngetCS value: %d", getCS());
    *(long*)tempEsp = 0;                    //eflags
    *(long*)(tempEsp+4) = getCS();          //cs
    *(long*)(tempEsp+8) = func;             //eip
    *(long*)(tempEsp+12) = 0;               //eax
    *(long*)(tempEsp+16) = 0;               //ecx
    *(long*)(tempEsp+20) = 0;               //edx
    *(long*)(tempEsp+24) = 0;               //ebx
    *(long*)(tempEsp+28) = new_memory + (getHeader -> size) + sizeof(struct memHeader); //esp
    *(long*)(tempEsp+32) = new_memory;      //ebp
    *(long*)(tempEsp+36) = 0;               //esi
    *(long*)(tempEsp+40) = 0;               //edi

    new_pcb -> state = READY;
    enqueuepcb(READY, new_pcb);
    return 1;
}