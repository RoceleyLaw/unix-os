/* syscall.c : syscalls
 */

#include <xeroskernel.h>

/* Your code goes here */
extern unsigned int syscreate( void (*func)(void), int stack ) {
    return 0;
}
extern void sysyield( void ) {
}
extern void sysstop( void ) {
    
}