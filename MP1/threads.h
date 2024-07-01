#ifndef THREADS_H_
#define THREADS_H_
#define NULL_FUNC ((void (*)(int))-1)
// TODO: necessary includes, if any
#include "user/setjmp.h"
// TODO: necessary defines, if any
#define max_task 20
struct task {
    void (*fun)(void *);
    void *arg;
    int task_set;
    jmp_buf env_task;
    void *stack_task;
    void *stack_task_p;
    int flag;
    struct task *previous;
    struct task *next;
};

struct thread {
    void (*fp)(void *arg);
    void *arg;
    void *stack;
    void *stack_p;
    jmp_buf env; // for thread function
    int buf_set; // 1: indicate jmp_buf (env) has been set, 0: indicate jmp_buf (env) not set
    int ID;
    int task_cnt;
    int flag;
    //void *now_stack;
    int now_fun; //0:thread, 1:task
    struct task *top;
    struct task *now_exc;
    struct thread *previous;
    struct thread *next;
};

struct thread *thread_create(void (*f)(void *), void *arg);
void thread_add_runqueue(struct thread *t);
void thread_yield(void);
void dispatch(void);
void schedule(void);
void thread_exit(void);
void thread_start_threading(void);
void task_exit(void);
// part 2
void thread_assign_task(struct thread *t, void (*f)(void *), void *arg);
#endif // THREADS_H_


