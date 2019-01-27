/* PCB implementation */
#include <xeroskernel.h>

pcb_t *stopped_queue;
pcb_t *ready_queue;
pcb_t *blocked_queue;
// for debugging purpose
pcb_t *running_process;
pcb_t pcb_table[32];

extern void initpcbtable() {
    kprintf("/n === initializing pcb table ======= /n");
    running_process = NULL;
    blocked_queue = NULL;
    size_t pcbtable_size = sizeof(pcb_table) / sizeof (pcb_t);
    kprintf("/n ##### pcb table size: %d ####### /n", pcbtable_size);

    stopped_queue = &pcb_table[0];
    kprintf("\n stopped_q: %d ", stopped_queue);
    for(int i = 0; i < pcbtable_size; i++) {
        // initialize the pcb table
        pcb_table[i].PID = i + 1;
        pcb_table[i].state = STOPPED;
        // next would only be used for queue
        // initialize the queue
        pcb_table[i].next = (pcb_t *)(&pcb_table[i+1]);
        // kprintf("\n AHHHHHHHHH : %d",  &pcb_table[i+1]);
    }
    int size = checkLinkedListLength(stopped_queue);
    kprintf("\n ^^^^^ assuming dead process initialized in stoppped_queue=>size: %d ^^", stopped_queue);
}

int checkLinkedListLength(void* head) {
   pcb_t *cur = head; 
   kprintf("^^^^^^^ cur : %d ^^^^^^^^^", cur);
   int count = 0;
   while(!cur) {
       cur = cur -> next;
       count++;
   }
   return count;
}
