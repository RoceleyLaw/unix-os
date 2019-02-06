/* syscall.c : syscalls
 */

#include <xeroskernel.h>
#include <stdarg.h>
/* Your code goes here */
static unsigned long process_id;
int syscall(int call, ... ) {
    // kprintf("\n Starting syscall type: %d \n", call);
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
    } else if (call == YIELD || call == STOP) {
        __asm__ __volatile__("\
            movl %0, %%eax \n\
            int $49 \n\
        "
        : 
        :"call"(call)
        : 
        );
    } else {
        kprintf("\n Error: unknown syscall type. Please check your syscall.c \n");
    }  
}

unsigned int syscreate(void (*func)(void), int stack ) {
    return syscall(CREATE, (unsigned long)func,(unsigned long) stack);
}

void sysyield(void) {
    syscall(YIELD);
}

void sysstop(void) {
    // kprintf("\n SysStop is called \n");
    // for (int i = 0; i < 100; i++) {}
    syscall(STOP);
}