/* mem.c : memory manager
 */

#include <xeroskernel.h>
#include <i386.h> // added header

/* Your code goes here */
// extern: variables can be accessed from any other file without being #include
extern long	freemem; 	/* start of free memory (set in i386.c) */
extern char	*maxaddr;	/* max memory address (set in i386.c)	*/
#define HOLESIZE        (600)
#define HOLESTART       (640 * 1024)
#define HOLEEND         ((1024 + HOLESIZE) * 1024)

/* The way to calculate the space we need */
// amnt = (req_sz)/16 + ((req_sz%16)?1:0); -> 
// if we have remainder, we need one more chunk
// amnt = amnt*16 + sizeof(struct memHeader); 

// struct size - 16 bytes
// address size - 4 bytes
struct memHeader {
// size - the size of the free slot + header
  unsigned long size;   // - 4 bytes
  struct memHeader *prev; // - 4 bytes
  struct memHeader *next; // - 4 bytes
  char *sanityCheck; // - 4 bytes
  
  // Used to track the address of the start of the actual space of the Node:
  // addr + sizeof(struct memHeader) (zero sized array trick)
  unsigned char dataStart[0];
};

// pointer(store the address) pointing to the start of the memory spot
// head pointer
struct memHeader *memSlot;

// called from the initproc() function 
// before any memory allocations occur
extern void kmeminit(void) {
    kprintf("\n !!!!!!== The max address is: %d =!!!!!! \n", maxaddr);
    kprintf("\n ==== The freemem pointer value is: %d ==== \n ", freemem);
    // Need to create 2 free nodes as we have hole in between the free memory space
    // head pointer points to the start of free space 
    memSlot = (struct memHeader *)(freemem);
    //kprintf("=== memSlot: %d; pointer size = %d; content size = %d ===", memSlot, sizeof(memSlot),sizeof(*memSlot));
    // free space before hitting the hole
    memSlot -> size = (unsigned long) (HOLESTART - freemem); // including the header size here
    memSlot -> prev = NULL;
    // pointing to the free space after skipping the hole
    memSlot -> next = (struct memHeader *) HOLEEND; // pointer = address??
    // set the next node's prev pointer
    memSlot -> next -> prev = memSlot;
    // reset the pointer pointing to 2nd new space
    memSlot = memSlot -> next;
    //kprintf("\n ==== The new 2nd pointer value is: %d ==== \n; HoleEnd is %d ", memSlot, HOLEEND);
    // Set the size of the 2nd new space
    memSlot -> size = (unsigned long) (maxaddr - HOLEEND);
    //kprintf("\n ==== SIZE of 2nd free space ==== %d\n; maxaddr is %d, its prev pointer = %d ", (*memSlot).size, HOLEEND, (*memSlot).prev);
    // Set the next pointer of the 2nd new space as NULL
    memSlot -> next = NULL;
    // memSlot points to the head, which refers to the start of the freemem
    memSlot = (struct memHeader *)freemem;
    kprintf("\n ==== The memSlot pointer value is: %d ==== \n ", memSlot);
    

    /* Called to debug the function. Should never be called here*/
    void* mem1 = kmalloc(1);
    void* mem2 = kmalloc(17);
    // Points directly to the data structure not the header
    int freed1 = kfree(mem1);
    void* mem3 = kmalloc(16);
    int freed2 = kfree(mem2);
    checkLinkedListSize();
    kprintf ("Calling mem1 allocation...", mem1);
    kprintf ("Calling mem2 allocation...", mem2);
}

void checkLinkedListSize(void) {
   struct memHeader *cur; 
   cur = memSlot;
   int count = 1;
   while(cur -> next != NULL) {
       kprintf("keep iterating");
       cur = cur -> next;
       count++;
   }
   kprintf("\n ==== The linked list size is: %d ==== \n ", count);
}

extern void *kmalloc(size_t size) {
    // kprintf("\n Allocating memory of size :%d \n", size);
    struct memHeader *cur = memSlot;
    unsigned long amnt = (size)/16 + ((size % 16)? 1:0);
    // kprintf("\n Number of paragraph allocated :%d \n", amnt);
    amnt = amnt * 16 + sizeof(struct memHeader);
    kprintf("\n Actual Memory allocated :%d \n", amnt);
    
    // if the size <= 0, ignore the request
    if (amnt <= 0) {
        kprintf("\n Requested a space <= 0 byte: %d byte", amnt);
        return 0;
    }

    while (cur != NULL) {
        // we will start from one case ---> add in loop afterwards
        if (cur -> size >= amnt) {
            // keep the original size (including the header size)
            unsigned long originalSize = cur -> size;
            // resize the current node
            // kprintf("\n old address of pointer %d, old size: %d \n", cur, cur -> size);
            cur -> size = amnt;
            /////// sanity check here ////////
            // seperate the rest as another chunk
            // pointer reset to the new address
            cur = (struct memHeader *)((unsigned long)memSlot + amnt); /// pay attention to this
            // Size of the free chunk after allocation
            cur -> size = originalSize - amnt;
            kprintf("\n new address of pointer %d, new size: %d \n", cur, cur -> size);
            
            cur -> next = memSlot -> next;
            cur -> prev = memSlot -> prev;

            // Clean up the pointers to avoid memory leaks
            memSlot -> next -> prev = NULL;
            memSlot -> next = NULL;
            memSlot -> prev = NULL;
            memSlot = cur; 

            // Can only allocate on 16 byte paragraph boundaries, last 4 bits would always be 0
            // ASSERT_EQUAL(((size_t)cur -> dataStart & 0xf), 0);
            return memSlot -> dataStart;
        } else {
            // if current node(free space) is too small, traverse the linked list
            // first-fit, not need to be best fit
            cur = cur -> next; 
        }
    }
    return 0;
}

extern int kfree(void *ptr) {
    // Need to put the released memory back into the free list
    // Merge adjacent blocks together to create larger contiguous regions
    // stub
    if (ptr == NULL || ptr > maxaddr || (ptr >= HOLESTART && ptr <= HOLEEND)) {
        kprintf("\n Error: ptr out of available range \n");
        return 0;
    }
    // memSlot points to the head of the linked list
    struct memHeader* cur = memSlot;
    
    // Find the last node on the linkedlist(freelist) and link the freed node to the list(freelist)
    while (cur -> next != NULL) {
        cur = cur -> next;
    }

    // ASSERT_EQUAL((cur -> next), NULL);
    // Need to test if the header is there
    cur -> next = (struct memHeader *) ((unsigned long)ptr - sizeof(struct memHeader));
    kprintf("\n the pointer value of the new added slot into free list: %d \n", cur -> next);
    return 1;
}