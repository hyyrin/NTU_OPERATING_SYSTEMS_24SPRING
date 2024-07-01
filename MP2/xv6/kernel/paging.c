#include "param.h"
#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "proc.h"

/* NTU OS 2024 */
/* Allocate eight consecutive disk blocks. */
/* Save the content of the physical page in the pte */
/* to the disk blocks and save the block-id into the */
/* pte. */
char *swap_page_from_pte(pte_t *pte) {
  char *pa = (char*) PTE2PA(*pte);
  uint dp = balloc_page(ROOTDEV);

  write_page_to_disk(ROOTDEV, pa, dp); // write this page to disk
  *pte = (BLOCKNO2PTE(dp) | PTE_FLAGS(*pte) | PTE_S) & ~PTE_V;

  return pa;
}

/* NTU OS 2024 */
/* Page fault handler */
int handle_pgfault() {
    /* Find the address that caused the fault */
    /* uint64 va = r_stval(); */
    
    /* TODO */
    uint64 va = r_stval();
    // round the address to page boundary
    va = PGROUNDDOWN(va);
    struct proc *p = myproc();
    pte_t *pte = walk(p->pagetable, va, 1);
    if (*pte & PTE_S){
        begin_op(); //read page from disk
        char *pa = kalloc();
        uint64 blockno = PTE2BLOCKNO(*pte);
        read_page_from_disk(ROOTDEV, pa, blockno);
        *pte = PA2PTE(pa) | PTE_FLAGS(*pte);
        bfree_page(ROOTDEV, blockno);
        end_op();
        //*pte = PA2PTE(pa) | PTE_FLAGS(*pte);
        *pte |= PTE_V; //set PTE_V bit
        *pte &= ~PTE_S; //unset PTE_S bit
    }
    else{
        char *pa = kalloc(); //allocate new page
        if (pa == 0) return -1; //out of memory
        memset(pa, 0, PGSIZE);
        int rtv = mappages(myproc()->pagetable, va, PGSIZE, (uint64)pa, PTE_W|PTE_X|PTE_R|PTE_U);
        if (rtv != 0){
            kfree((void*)pa);
            return -1;
        }
    }
    return 0; //allocate successfully
    //panic("not implemented yet\n");
}
