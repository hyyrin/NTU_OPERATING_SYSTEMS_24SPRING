#include "fifo.h"

#include "param.h"
#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "proc.h"

void q_init(queue_t *q){
    q->size = 0;
	//panic("Not implemented yet\n");
}

int q_push(queue_t *q, uint64 e){
	//panic("Not implemented yet\n");
    if (q->size >= PG_BUF_SIZE) return -1;
    q->bucket[q->size++] = e;
    return 0;
}

uint64 q_pop_idx(queue_t *q, int idx){
    if (idx < 0 || idx >= q->size) return -1;
    uint64 e = q->bucket[idx];
    for (int i = idx; i < q->size - 1; i++)
        q->bucket[i] = q->bucket[i + 1];
    q->size--;
    return e;
	//panic("Not implemented yet\n");
}

int q_empty(queue_t *q){
    return (q->size == 0);
	//panic("Not implemented yet\n");
}

int q_full(queue_t *q){
    return (q->size >= PG_BUF_SIZE);
	//panic("Not implemented yet\n");
}

int q_clear(queue_t *q){
    q->size = 0;
    return 0;
	//panic("Not implemented yet\n");
}

int q_find(queue_t *q, uint64 e){
    for (int i = 0; i < q->size; i++){
        if (q->bucket[i] == e) return i;
    }
    return -1;
	//panic("Not implemented yet\n");
}
