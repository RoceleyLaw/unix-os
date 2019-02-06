/* syscall.c : syscalls
 */

#include <xeroskernel.h>
#include <stdarg.h>
/* Your code goes here */
static unsigned long process_id;
extern int syscall( int call, ... ) {
    kprintf("Starting syscall type: %d", call);
    if (call == CREATE) {
        va_list ap;
        va_start(ap, call);
        void (*func)(void) = va_arg(ap, void(*)(void));
        int stack = va_arg(ap, int);
        va_end(ap);
        __asm__ __volatile__("\
            movl %0, %%eax \n\
            movl %1, %%ecx \n\
            movl %2, %%edx \n\
            int $49 \n\
            movl %%eax, process_id \n\
        "
        : 
        :"call"(call), "funcptr"(func), "stack"(stack)
        : 
        );
        return (int)process_id;
    } else if (call == YIELD || call == STOPPED) {
        __asm__ __volatile__("\
            movl %0, %%eax \n\
            int $49 \n\
        "
        : 
        :"call"(call)
        : 
        );
        return (int)process_id
    }
    kprintf("\n Error: unknown syscall type. Please check your syscall.c \n");
	return -1;  
}

extern unsigned int syscreate(void (*func)(void), int stack ) {
    return syscall(CREATE, (unsigned long)func,(unsigned long) stack);
}
extern void sysyield( void ) {
    syscall(YIELD);
}
extern void sysstop(void) {
    kprintf("\n SysStop is called \n");
    // for (int i = 0; i < 100; i++) {}
    syscall(STOP);
}