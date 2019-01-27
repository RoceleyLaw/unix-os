/* xeroskernel.h - disable, enable, halt, restore, isodd, min, max */

#ifndef XEROSKERNEL_H
#define XEROSKERNEL_H

/* Symbolic constants used throughout Xinu */

typedef	char    Bool;        /* Boolean type                  */
typedef unsigned int size_t; /* Something that can hold the value of
                              * theoretical maximum number of bytes 
                              * addressable in this architecture.
                              */
#define	FALSE   0       /* Boolean constants             */
#define	TRUE    1
#define	EMPTY   (-1)    /* an illegal gpq                */
#define	NULL    0       /* Null pointer for linked lists */
#define	NULLCH '\0'     /* The null character            */


/* Universal return constants */

#define	OK            1         /* system call ok               */
#define	SYSERR       -1         /* system call failed           */
#define	EOF          -2         /* End-of-file (usu. from read)	*/
#define	TIMEOUT      -3         /* time out  (usu. recvtim)     */
#define	INTRMSG      -4         /* keyboard "intr" key pressed	*/
                                /*  (usu. defined as ^B)        */
#define	BLOCKERR     -5         /* non-blocking op would block  */

/* Functions defined by startup code */


void           bzero(void *base, int cnt);
void           bcopy(const void *src, void *dest, unsigned int n);
void           disable(void);
unsigned short getCS(void);
unsigned char  inb(unsigned int);
void           init8259(void);
int            kprintf(char * fmt, ...);
void           lidt(void);
void           outb(unsigned int, unsigned char);
void           set_evec(unsigned int xnum, unsigned long handler);


/* Anything you add must be between the #define and this comment */
typedef enum {
    READY = 0,
    RUNNING = 1,
    BLOCKED,
    STOPPED
} process_state_enum_t;

typedef struct context_frame {
    unsigned long edi;
    unsigned long esi;
    unsigned long ebp;
    unsigned long esp;
    unsigned long ebx;
    unsigned long edx;
    unsigned long ecx;
    unsigned long eax;
    unsigned long iret_eip;
    unsigned long iret_cs;
    unsigned long eflags;
} context_frame_t;

// Is our context our CPU state as well ??
struct CPU {
    context_frame_t *context;
};

typedef struct pcb {
    int PID;
    process_state_enum_t state;
    // int parent_pid; // Not required for our kernel
    // struct CPU cpu_state; // CPU context part 1: IP, stack, registers, process flags
    // we only need to store the stack ptr. Context would be stored on top of process stack
    unsigned long *esp;
    struct pcb *next;
} pcb_t;

// Global static array of size 32 (max process we need to accommodate)
extern pcb_t pcb_table[32];

extern void kmeminit(void);
extern void *kmalloc(size_t size);
extern int kfree(void *ptr);
int checkLinkedListSize(void);
extern void dispatch();
extern int contextswitch(pcb_t *p);
extern void contextinit ();
extern int create( void (*func)(void), int stack );

// syscall.c
extern unsigned int syscreate( void (*func)(void), int stack );
extern void sysyield(void);
extern void sysstop(void);

// pcb.c
extern void initpcbtable(void);
#endif
