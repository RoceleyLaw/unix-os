/* user.c : User processes
 */

#include <xeroskernel.h>

/* Your code goes here */
/* user.c : User processes
 */
void producer(void);
void consumer(void);
/* Root process */
void root(void) {
    kprintf("\n Hello world!");
    syscreate(&producer, 8194);
    syscreate(&consumer, 8194);
    for (;;) sysyield();
}

/* Producer */
void producer(void) {
    for (int i = 0; i < 15; i++) {
        kprintf("\n Happy 2019\n");
        sysyield();
    }
   sysstop();    
}

/* Consumer */
void consumer(void) {
    for (int i = 0; i < 15; i++) {
        kprintf("everyone \n");
        sysyield();
    }
    sysstop();  
}