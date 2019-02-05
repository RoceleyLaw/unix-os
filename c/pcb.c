/* PCB implementation */
#include <xeroskernel.h>

pcb_t *stopped_queue_head;
pcb_t *ready_queue_head;
pcb_t *blocked_queue_head;
// keep tails to facilitate enqueue: O(1)
pcb_t *stopped_queue_tail;
pcb_t *ready_queue_tail;
pcb_t *blocked_queue_tail;
// for debugging purpose
pcb_t *running_process;
pcb_t pcb_table[32];

extern void initpcbtable() {
    kprintf("/n === initializing pcb table ======= /n");
    running_process = NULL;
    blocked_queue_head = NULL;
    blocked_queue_tail = NULL;
    ready_queue_head = NULL;
    ready_queue_tail = NULL;
    size_t pcbtable_size = sizeof(pcb_table) / sizeof (pcb_t);
    kprintf("/n ##### pcb table size: %d ####### /n", pcbtable_size);

    stopped_queue_head = &pcb_table[0];
    kprintf("\n stopped_q: %d ", stopped_queue_head);
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
            stopped_queue_tail = (pcb_t *)(&pcb_table[i]);
        }
    }
    int size = checkLinkedListLength(stopped_queue_head);
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

extern pcb_t *dequeuepcb(process_state_enum_t state) {
    pcb_t *cur = NULL;
    if (state == STOPPED) {
        cur = stopped_queue_head;
        stopped_queue_head = stopped_queue_head -> sq_next;
        cur -> sq_next = NULL;
        int size = checkLinkedListLength(stopped_queue_head);
        kprintf("\n ^^^ remaining size: %d", size);
        kprintf("\n ^^^^  dequeue PID: %d", cur -> PID);
        // if only one pcb
        if (stopped_queue_head == stopped_queue_tail) {
            stopped_queue_tail = NULL;  
        }
        return cur;
    } else if (state == READY) {
        cur = ready_queue_head;
        ready_queue_head = ready_queue_head -> rq_next;
        cur -> rq_next = NULL;
        int size = checkLinkedListLength(ready_queue_head);
        kprintf("\n ^^^ remaining size: %d", size);
        kprintf("\n ^^^^  dequeue PID: %d", cur -> PID);
        if (ready_queue_head == ready_queue_tail) {
            ready_queue_tail = NULL;  
        }
        return cur;
    } else if (state == BLOCKED) {
        cur = blocked_queue_head;
        blocked_queue_head = blocked_queue_head -> bq_next;
        cur -> bq_next = NULL;
        // int size = checkLinkedListLength(blocked_queue);
        // kprintf("\n ^^^ remaining size: %d", size);
        // kprintf("\n ^^^^  dequeue PID: %d", cur -> PID);
        if (blocked_queue_head == blocked_queue_tail) {
            blocked_queue_tail = NULL;  
        }
        return cur;
    } else {
        kprintf("\n Error: dequeue failure!");
        return NULL;
    }
    return NULL;
}

extern void enqueuepcb(process_state_enum_t state, pcb_t *new_pcb) {
    if (state == STOPPED) {
        if (stopped_queue_tail) {
            stopped_queue_tail -> sq_next = new_pcb;
            stopped_queue_tail = new_pcb;
        } else {
            stopped_queue_head = new_pcb;
            stopped_queue_tail = new_pcb;
        }
        int size = checkLinkedListLength(stopped_queue_head);
        kprintf("\n ^^^ enq sq remaining size: %d", size);
        kprintf("\n ^^^^  enq sq PID: %d", new_pcb -> PID);
    } else if (state == READY) {
        if (ready_queue_head) {
            ready_queue_tail -> rq_next = new_pcb;
            ready_queue_tail = new_pcb;
        } else {
            ready_queue_head = new_pcb;
            ready_queue_tail = new_pcb;
        }
        int size = checkLinkedListLength(ready_queue_head);
        kprintf("\n ^^^ enq rq remaining size: %d", size);
        kprintf("\n ^^^^  enq rq PID: %d", new_pcb -> PID);
    } else if (state == BLOCKED) {
        if (blocked_queue_head) {
            blocked_queue_tail -> bq_next = new_pcb;
            blocked_queue_tail = new_pcb;
        } else {
            blocked_queue_head = new_pcb;
            blocked_queue_tail = new_pcb;
        }
        int size = checkLinkedListLength(blocked_queue_head);
        kprintf("\n ^^^ enq bq remaining size: %d", size);
        kprintf("\n ^^^^  enq bq PID: %d", new_pcb -> PID);
    } else {
        kprintf("\n Error: enqueue failure!");
    }
}