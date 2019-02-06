/* syscall.c : syscalls
 */

#include <xeroskernel.h>
#include <stdarg.h>
/* Your code goes here */
extern int syscall( int call, ... ) {

    if (call == SYSCALL_CREATE) {
        va_list ap;
        va_start(ap);
        int result = 0;
        __asm__ __volatile__("\
            movl %1, %%eax \n\
            movl %2, %%edx \n\
            int $66 \n\
            movl %%eax, %0 \n\
        "
        : "=r" (result)
        :	"r" (call), "r" (ap)
        : "%eax", "%edx"
        );
        va_end(ap);
    }
	return result;
}

extern unsigned int syscreate( void (*func)(void), int stack ) {
    return syscall(SYSCALL_CREATE, (unsigned long)func,(unsigned long) stack);
}
extern void sysyield( void ) {
}
extern void sysstop( void ) {
}