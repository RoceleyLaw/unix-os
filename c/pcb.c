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
        // next would only be used for queue(linked list structure)
        // initialize the queue
        // do not populate next of the last element
        if (i != pcbtable_size - 1) {
            pcb_table[i].sq_next = (pcb_t *)(&pcb_table[i+1]);
        } else {
            pcb_table[i].sq_next = NULL;
        }
    }
    int size = checkLinkedListLength(stopped_queue);
    kprintf("\n ^^^^^ assuming dead process initialized in stoppped_queue=>size: %d ^^", size);
}

int checkLinkedListLength(void* head) {
   pcb_t *cur = head; 
   int count = 0;
   while(cur != NULL) {
       cur = cur -> sq_next;
       // kprintf("\n cur: %d", cur);
       count++;
   }
   return count;
}

extern int dequeuepcb(process_state_enum_t state) {
    pcb_t *cur = NULL;
    if (state == STOPPED) {
        cur = stopped_queue;
        stopped_queue = stopped_queue -> sq_next;
        cur -> sq_next = NULL;
        int size = checkLinkedListLength(stopped_queue);
        kprintf("\n ^^^ remaining size: %d", size);
        kprintf("\n ^^^^  dequeue PID: %d", cur -> PID);
        return cur -> PID;
    } else if (state == READY) {
        cur = ready_queue;
        ready_queue = ready_queue -> rq_next;
        cur -> rq_next = NULL;
        // int size = checkLinkedListLength(ready_queue);
        // kprintf("\n ^^^ remaining size: %d", size);
        // kprintf("\n ^^^^  dequeue PID: %d", cur -> PID);
        return cur -> PID;
    } else if (state == blocked_queue) {
        cur = blocked_queue;
        blocked_queue = blocked_queue -> bq_next;
        cur -> bq_next = NULL;
        // int size = checkLinkedListLength(blocked_queue);
        // kprintf("\n ^^^ remaining size: %d", size);
        // kprintf("\n ^^^^  dequeue PID: %d", cur -> PID);
        return cur -> PID;
    } else {
        kprintf("\n Error: dequeue failure!");
        return -1;
    }
    return -1;
}

int dequeuepcbHelper(pcb_t *queue, pcb_t *next) {
    pcb_t *cur = queue;
    cur = cur -> next;
    queue -> next = NULL;
    queue = cur;
    kprintf("\n ^^^^  dequeue PID: %d", queue -> PID);
    return queue -> PID;
}
