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
 void printContext(unsigned long context_frame){
     for(int i = 0; i < 11; i++){
         kprintf("%d\n",  *(unsigned long*)(context_frame + i*4));
     }
 }
static void *k_stack;
static unsigned long ESP;
extern void contextinit () {
    set_evec(SYS_CALL_INT_NUM, _ISREntryPoint);
}
extern int contextswitch(pcb_t *p) {
ESP = p -> esp;
printContext(ESP);
for(;;);
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
    popa \n\
    popf \n\
        "
  :
  : 
  : "%eax"
  );
 kprintf("\n the  value of updated stack ptr after ctsw: %d", p -> esp);
 p -> esp = ESP;

 return 0;


}
