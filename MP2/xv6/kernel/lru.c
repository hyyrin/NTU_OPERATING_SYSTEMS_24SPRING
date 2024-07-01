#include "lru.h"

#include "param.h"
#include "types.h"
#include "memlayout.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include "proc.h"

void lru_init(lru_t *lru){
    lru->size = 0;
	//panic("not implemented yet\n");
}

int lru_push(lru_t *lru, uint64 e){
    if (lru->size >= PG_BUF_SIZE) return -1;
    lru->bucket[lru->size++] = e;
    return 0;
	//panic("not implemented yet\n");
}

uint64 lru_pop(lru_t *lru, int idx){
    if (idx < 0 || idx >= lru->size) return -1;
    uint64 e = lru->bucket[idx];
    for (int i = idx; i < lru->size - 1; i++)
        lru->bucket[i] = lru->bucket[i + 1];
    lru->size--;
    return e;
	//panic("not implemented yet\n");
}

int lru_empty(lru_t *lru){
    return (lru->size == 0);
	//panic("not implemented yet\n");
}

int lru_full(lru_t *lru){
    return (lru->size >= PG_BUF_SIZE);
	//panic("not implemented yet\n");
}

int lru_clear(lru_t *lru){
    lru->size = 0;
    return 0;
	//panic("not implemented yet\n");
}

int lru_find(lru_t *lru, uint64 e){
    for (int i = 0; i < lru->size; i++){
        if (lru->bucket[i] == e) return i;
    }
    return -1;
	//panic("not implemented yet\n");
}
