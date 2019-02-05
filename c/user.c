/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */
/* user.c : User processes
 */

/* Root process */
extern void root(void) {
    kprintf("Hello world!\n");
    //TODO: stack size is set randomly as 100, need to think about it
    // syscreate(&producer, 100);
    // syscreate(&consumer, 100);
    // for (;;) sysyield();
}

/* Producer */
// void producer(void) {
//     for (int i = 0; i < 15; i++) {
//         kprintf("\n Happy 2019\n");
//         sysyield();
//     }
//     sysstop();    
// }

// /* Consumer */
// void consumer(void) {
//     for (int i = 0; i < 15; i++) {
//         kprintf("everyone \n");
//         sysyield();
//     }
//     sysstop();  
// }