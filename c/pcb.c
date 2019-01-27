/* PCB implementation */
#include <xeroskernel.h>

extern pcb_t *stopped_queue;
extern pcb_t *ready_queue;
extern pcb_t *blocked_queue;
// for debugging purpose
extern pcb_t *running_process;

extern void initpcbtable() {
    kprintf("/n === initializing pcb table ======= /n");
    running_process = NULL;
    blocked_queue = NULL;
    size_t pcbtable_size = sizeof(pcb_table) / sizeof (pcb_t);
    kprintf("/n ##### pcb table size: %d ####### /n", pcbtable_size);
}
