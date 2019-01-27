/* PCB implementation */
#include <xeroskernel.h>

pcb_t *stopped_queue;
pcb_t *ready_queue;
pcb_t *blocked_queue;
// for debugging purpose
pcb_t *running_process;

extern void initpcbtable() {
    kprintf("\n === initializing pcb table ======= \n");
    running_process = NULL;
    blocked_queue = NULL;
    size_t pcbtable_size = sizeof(pcb_table) / sizeof (pcb_t);
    kprintf("\n ##### pcb table size: %d ####### \n", pcbtable_size);

    pcb_t *cur = stopped_queue;
    for(int i = 0; i < pcbtable_size; i++) {
        cur -> PID = i + 1;
        cur -> state = STOPPED;
        kprintf("\n ^^^^^ assuming dead process initialized in stoppped_queue=> PID: %d, cur: %d ^^", cur -> PID, cur);
        cur = cur -> next;
    }
    // Avoid dangling ptr
    cur = NULL;
}
