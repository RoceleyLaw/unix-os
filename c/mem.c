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
    // Need to create 2 free nodes as we have hole in between the free memory space
    // head pointer points to the start of free space 
    // memSlot = (struct memHeader *)(freemem);
    // Address must align to 16
    memSlot = freemem + (freemem % 16);
    // kprintf("=== memSlot: %d; pointer size = %d; content size = %d ===", memSlot, sizeof(memSlot),sizeof(*memSlot));
    // free space before hitting the hole
    memSlot -> size = (unsigned long) (HOLESTART - (unsigned long)memSlot); // including the header size here
    memSlot -> prev = NULL;
    // Free blocks would have sanityCheck value NULL
    memSlot -> sanityCheck = NULL;

    kprintf("=== free memory chunk 1 size: %d; ==", memSlot -> size);
    // pointing to the free space after skipping the hole
    memSlot -> next = (struct memHeader *) HOLEEND; // pointer = address??
    // set the next node's prev pointer
    memSlot -> next -> prev = memSlot;
    // reset the pointer pointing to 2nd new space
    memSlot = memSlot -> next;
    // kprintf("\n ==== The new 2nd pointer value is: %d ==== \n; HoleEnd is %d ", memSlot, HOLEEND);
    // Set the size of the 2nd new space
    memSlot -> size = (unsigned long) (maxaddr - HOLEEND + 1);
    // kprintf("\n ==== SIZE of 2nd free space ==== %d\n; maxaddr is %d, its prev pointer = %d ", (*memSlot).size, HOLEEND, (*memSlot).prev);
    // Set the next pointer of the 2nd new space as NULL
    memSlot -> next = NULL;
    memSlot -> sanityCheck = NULL;
    // memSlot points to the head, which refers to the start of the freemem
    memSlot = (struct memHeader *)(freemem + (freemem % 16));
    kprintf("\n ==== The memSlot pointer value is: %d ==== \n ", memSlot);
    
   
    /* Called to debug the function. Should never be called here*/
    void* mem1 = kmalloc(1);
    void* mem2 = kmalloc(17);
    // Points directly to the data structure not the header
    int freed1 = kfree(mem1);
    void* mem3 = kmalloc(16);
    // kprintf ("Calling mem3 allocation...: %d", mem3);
    void* mem4 = kmalloc(620300);
    kprintf ("Calling free2 allocation...: %d", mem2);
    int freed2 = kfree(mem3);
    int free4 = kfree(mem2);
    // free the same spot twice to check sanitycheck value
    int free5 = kfree(mem2);
    int freed6 = kfree(mem3);
    int freed3 = kfree((unsigned long)mem2 + 10);
    // size that fits exactly the space left in the first segment of free mem
    void* mem6 = kmalloc(1911008 - 16);
    void* mem5 = kmalloc(620240 - 16); 
    //int free7 = kfree(mem2);
    //int freed8 = kfree(mem3);
    int freed9 = kfree(mem1);
    void* mem8 = kmalloc(1);
    void* mem9 = kmalloc(17);
    int count = checkLinkedListSize(memSlot);
    //kprintf ("\n Calling mem1 allocation...: %d", mem1);
    //kprintf ("Calling mem2 allocation...", mem2);
}

// Helper function for testing 
extern int checkLinkedListSize(void* head) {
   void *cur; 
   // cur = memSlot;
    cur = head;
//    if (cur) {
//        kprintf("\n ==== The linked list addr is: %d, size is: %d ==== \n ", cur, cur -> size);
//    } else {
//        kprintf("\n ==== The linked list addr is: %d, size is 0 ", cur);
//        return 0;
//    }
   int count = 0;
   while(cur) {
       kprintf("keep iterating");
       cur = cur -> next;
       count++;
       // kprintf("\n ==== The linked list addr is: %d , size is: %d ==== \n ", cur, cur -> size);
   }
   kprintf("\n ==== The linked list size is: %d ==== \n ", count);
   return count;
}

extern void *kmalloc(size_t size) {
    // ASSERTION: make sure ptr is pointer to an address align to 16
    // assert((unsigned long) memSlot % 16 == 0);
    kprintf("\n memSlot :%d \n", memSlot);
    struct memHeader *cur = memSlot;
    struct memHeader *tmp = memSlot;
    unsigned long amnt = (size) / 16 + ((size % 16)? 1:0);
    // kprintf("\n Number of paragraph allocated :%d \n", amnt);
    amnt = (amnt * 16) + sizeof(struct memHeader);
    
    // ASSERTION: amnt allocated must be a multiple of 16
    // assert(amnt % 16 == 0);
    kprintf("\n Actual Memory allocated :%d \n", amnt);
    
    // if the size <= 0, ignore the request
    if (amnt <= 0) {
        kprintf("\n Requested a space <= 0 byte: %d byte", amnt);
        return 0;
    }

    while (cur != NULL) {
        if (cur -> size == amnt) {
            if (cur -> prev) {
                cur -> prev -> next = cur -> next; 
            } else {
                // cur -> prev == NULL means the slot is allocated at the head of the freelist
                // if there is only one big slot in the freelist and it is allocated now, memSlot would
                // point to null. 
                memSlot = cur -> next;
            }
            if (cur -> next) {
                cur -> next -> prev = cur -> prev;   
            }

            // Make the doubly linked ptrs NULL here so that this slot is not in the freelist anymore
            cur -> prev = NULL;
            cur -> next = NULL;
            cur -> sanityCheck = cur;
            kprintf("\n ===== allocated block sanityCheck value: %d =======", cur -> sanityCheck);
            return cur -> dataStart;
        } else if (cur -> size > amnt) {
            tmp = cur;
            // keep the original size (including the header size)
            unsigned long originalSize = cur -> size;
            // resize the current node
            // kprintf("\n old address of pointer %d, old size: %d \n", cur, cur -> size);
            cur -> size = amnt;
            // Any potential risk for setting sanityCheck as a pointer pointing to the start of header?
            /////// sanity check here ////////
            cur -> sanityCheck = cur;
            // seperate the rest as another chunk
            // pointer reset to the new address
            cur = (struct memHeader *)((unsigned long)cur + amnt); /// pay attention to this
            // kprintf("------ cur pointer value after redirect to next chunk------: %d", cur);
            // Size of the free chunk + header after allocation
            cur -> size = originalSize - amnt;
            // kprintf("\n new address of pointer %d, new size: %d \n", cur, cur -> size);
            kprintf("\n sanity check pointer value: %d, address of header: %d \n", cur -> sanityCheck, cur);
            
            cur -> next = tmp -> next;
            cur -> prev = tmp -> prev;
            kprintf("cur->prev: %d", cur->prev);

            if (cur -> prev) {
                cur -> prev -> next = cur;
            }
            kprintf("cur->next: %d", cur->next);
            if (cur -> next) {
                cur -> next -> prev = cur;
            }
            
            // Clean up the pointers to avoid memory leaks
            tmp -> next = NULL;
            // if the head of the freelist has been (partially) filled, move the head of the list
            if (!(tmp -> prev)) {
                memSlot = cur;
            }
            kprintf("the adjusted pointer value: %d", memSlot);
            tmp -> prev = NULL;
            // tmp = NULL; 
            return tmp -> dataStart;
        } else {
            // if current node(free space) is too small, traverse the linked list
            // first-fit, not need to be best fit
            cur = cur -> next; 
            kprintf("------ cur pointer value ------: %d", cur);
        }
    }
    kprintf("Warning: malloc failed due to insufficient memory space");
    return 0;
}

// TODO: Reset sanitycheck values after freed
extern int kfree(void *ptr) {
    // Need to put the released memory back into the free list
    // Merge adjacent blocks together to create larger contiguous regions
    // stub
    if (ptr == NULL || ptr > maxaddr || (ptr >= HOLESTART && ptr < HOLEEND)) {
        kprintf("\n Error: ptr out of available range. \n");
        return 0;
    }
    /* Sanity Check here */
    // Check the value here
    // Find the starting addr of the header 
    struct memHeader *startOfHeader = (struct memHeader *) ((unsigned long)ptr - sizeof(struct memHeader));
    kprintf("\n ~~~~~~~startOfHeader: %d", startOfHeader);
    if ((strcmp(startOfHeader -> sanityCheck, startOfHeader)) != 0) {
        kprintf("\n Error: no header found at the ptr address. \n");
        return 0;
    }
    // memSlot points to the head of the linked list
    struct memHeader* cur1 = memSlot;
    struct memHeader* tail = memSlot;
    // flag to check if the freed block has been merged or not
    int mergedPrev = 0;
    int mergedNext = 0;

    // Why 2 loops here? => we want to always merge the next block before we merge the prev adjacent block
    while (cur1 != NULL) {
        // coalesce freed blocks adjacent to each other
        // Merge needed: merge the next block
        if ((unsigned long)startOfHeader + (startOfHeader -> size) == (unsigned long)cur1) {
            startOfHeader -> size = (startOfHeader -> size) + (cur1 -> size);
            mergedNext = 1;
            /* important logic */
            kprintf("\n Merging the next....: %d", cur1);
            if (cur1 -> next) {
                cur1 -> next -> prev = startOfHeader;
            }
            if (cur1 -> prev) {
                cur1 -> prev -> next = startOfHeader;
            } else {
                // if cur1 -> prev == NULL, that means we are merging the head of freelist
                memSlot = startOfHeader;
            }

            startOfHeader -> prev = cur1 -> prev;
            startOfHeader -> next = cur1 -> next;
            startOfHeader -> sanityCheck = NULL;
            cur1 -> prev = NULL;
            cur1 -> next = NULL;
            cur1 -> sanityCheck = NULL;
            break;
        }
        cur1 = cur1 -> next;
        // keep a tail ptr for no merge case
        if (tail -> next != NULL) {
            tail = tail -> next;
        }
    }

    // note: putting cur2 here or before the above loop would make a significant value
    struct memHeader* cur2 = memSlot;
    while (cur2 != NULL) { 
         // Merge needed: merge the prev block
         kprintf("\n What the fuck????? .... memSlot: %d, cur2 + cur2->size: %d, %d", memSlot, (unsigned long)cur2 + (cur2 -> size), startOfHeader);
        if ((unsigned long)cur2 + (cur2 -> size) == (unsigned long)startOfHeader) {
            kprintf("\n Merging the prev....: %d", cur2);
            cur2 -> size = (cur2 -> size) + (startOfHeader -> size);
            // The user should not be able to free this area again
            startOfHeader -> sanityCheck = NULL;
            
            // remove it from the free list if it is coming from the loop above
            // which means:
            // - merge is done for the next block
            // - the merged area is in the freelist
            // - merge needs to be done for the prev block now
            // - the node we are removing would be the startOfHeader node
            if (startOfHeader -> prev) {
                startOfHeader -> prev -> next = startOfHeader -> next;
            } else {
                // if startOfHeader is the head of the list, move the head
                if ((unsigned long) memSlot == (unsigned long) startOfHeader) {
                    memSlot = startOfHeader -> next;
                }   
            }

            if (startOfHeader -> next) {
                startOfHeader -> next -> prev = startOfHeader -> prev;
            }
            startOfHeader -> sanityCheck = NULL;
            startOfHeader -> prev = NULL;
            startOfHeader -> next = NULL;
            mergedPrev = 1;
            break;
        }
        kprintf("cur2 next!!!!!!!: %d", cur2 -> next);
        cur2 = cur2 -> next;
     }

    // NO merge needed: Find the last node on the linkedlist(freelist) and link the freed node to the list(freelist)
    // ASSERT_EQUAL((cur -> next), NULL);
    if (!mergedPrev && !mergedNext) {
        tail -> next = startOfHeader;
        tail -> next -> prev = tail;
        tail -> next -> next = NULL;
        tail -> sanityCheck = NULL;                                                                             NULL;
    }
    /* Sanity Check here */
    // Do I need to reset sanity check value here?
    
    // kprintf("\n the pointer value of the new added slot into free list: %d \n", cur -> next);
    return 1;
}