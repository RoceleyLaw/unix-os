/* PCB implementation */
#include <xeroskernel.h>

pcb_t *stopped_queue;
pcb_t *ready_queue;
pcb_t *blocked_queue;
// for debugging purpose
pcb_t *running_process;

extern void initpcbtable() {
    kprintf("/n === initializing pcb table ======= /n");
    running_process = NULL;
    blocked_queue = NULL;
    size_t pcbtable_size = sizeof(pcb_table) / sizeof (pcb_t);
    kprintf("/n ##### pcb table size: %d ####### /n", pcbtable_size);

    pcb_t *cur = stopped_queue;
    for(int i = 0; i < pcbtable_size; i++) {
        cur -> PID = i + 1;
        cur -> state = STOPPED;
        cur = cur -> next;
    }
    int size = checkLinkedListLength(stopped_queue);
    kprintf("\n ^^^^^ assuming dead process initialized in stoppped_queue=> PID: %d, cur: %d ^^", cur -> PID, cur);
    // Avoid dangling ptr
    cur = NULL;
}

int checkLinkedListLength(void* head) {
    pcb_t *cur = head; 
   // cur = memSlot;

//    if (cur) {
//        kprintf("\n ==== The linked list addr is: %d, size is: %d ==== \n ", cur, cur -> size);
//    } else {
//        kprintf("\n ==== The linked list addr is: %d, size is 0 ", cur);
//        return 0;
//    }
   int count = 0;
   while(!cur) {
       kprintf("keep iterating");
       cur = cur -> next;
       count++;
       kprintf("\n ==== The linked list addr is: %d , size is: %d ==== \n ", cur, cur -> size);
   }
   kprintf("\n ==== The linked list size is: %d ==== \n ", count);
   return count;
}
