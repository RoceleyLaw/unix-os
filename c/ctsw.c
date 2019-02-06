/* ctsw.c : context switcher
 */

#include <xeroskernel.h>
/* Your code goes here - You will need to write some assembly code. You must
   use the gnu conventions for specifying the instructions. (i.e this is the
   format used in class and on the slides.) You are not allowed to change the
   compiler/assembler options or issue directives to permit usage of Intel's
   assembly language conventions.
*/

void _ISREntryPoint();
static void *k_stack;
static unsigned long ESP;
static unsigned long ECX;
static unsigned long EDX;
extern void contextinit () {
    set_evec(SYS_CALL_INT_NUM, _ISREntryPoint);
}
extern int args[2];
extern int contextswitch(pcb_t *p) {
ESP = p -> esp;
for(int i=0; i<1000000; i++);
kprintf("\n the  value of stack ptr: %d", ESP);
  __asm __volatile( " \
    pushf  \n\
    pusha  \n\
    movl  %%esp, k_stack   \n\  
    movl  ESP, %%esp  \n\
    popa  \n\
    iret  \n\
    _ISREntryPoint:  \n\
    pusha   \n\
    movl  %%esp, ESP  \n\
    movl  k_stack, %%esp \n\
    movl  %%ecx,  ECX \n\
    movl  %%edx,  EDX \n\
    popa \n\
    popf \n\
        "
  :
  : 
  : "%eax"
  );
 kprintf("\n the  value of updated stack ptr after ctsw: %d", p -> esp);
p -> esp = ESP;
context_frame_t* cf = (context_frame_t *)p -> esp;
// store function ptr as the first argument
arg[0] = ECX;
// store the stack size as the second argument
arg[1] = EDX;
kprintf("\n cf -> eax value : %d", cf -> eax);
for (int i = 0; i < 1000; i++);
return (int)cf -> eax;
}