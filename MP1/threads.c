#include "kernel/types.h"
#include "user/setjmp.h"
#include "user/threads.h"
#include "user/user.h"
#define NULL 0
#define max_task 20

static struct thread* current_thread = NULL;
static int id = 1;
static jmp_buf env_st;
// static jmp_buf env_tmp;
//highest score
struct thread *thread_create(void (*f)(void *), void *arg){
    struct thread *t = (struct thread*) malloc(sizeof(struct thread));
    unsigned long new_stack_p;
    unsigned long new_stack;
    new_stack = (unsigned long) malloc(sizeof(unsigned long)*0x100);
    new_stack_p = new_stack +0x100*8-0x2*8;
    t->fp = f;
    t->arg = arg;
    t->ID  = id;
    t->buf_set = 0;
    t->stack = (void*) new_stack;
    t->stack_p = (void*) new_stack_p;
    //t->now_stack = (void*) new_stack;
    id++;
    //part2
    t->flag = 0;
    t->now_fun = -1;
    t->task_cnt = 0;
    t->top = NULL;
    t->now_exc = NULL;
    return t;
}
void thread_add_runqueue(struct thread *t){
    if(current_thread == NULL){
        // TODO
        current_thread = t;
        current_thread->next = t;
        current_thread->previous = t;
    }
    else{
        // TODO
        struct thread *tmp = current_thread->previous;
        tmp->next = t;
        t->previous = tmp;
        t->next = current_thread;
        current_thread->previous = t;
    }
}
void thread_yield(void){
    // TODO
    //part1 + part
    if (current_thread->now_fun == 1){ //now is task function
        if (setjmp(current_thread->now_exc->env_task)){
            return;
        }
    }
    else{ //now is thread_function
        if (setjmp(current_thread->env)) {
            return;
        }
    }
    schedule();
    dispatch();
}


void dispatch(void) {
    // Check if there are any tasks to execute.
    if (current_thread->top != NULL){
        
        if (current_thread->top->task_set) longjmp(current_thread->top->env_task, 1);
        else {
            if (setjmp(current_thread->top->env_task) == 0) {
                current_thread->top->env_task->sp = (unsigned long)current_thread->top->stack_task_p;
                //current_thread->top->env_task->sp = (unsigned long)current_thread->stack_p;
                longjmp(current_thread->top->env_task, 1);
            }
            else{
                current_thread->top->task_set = 1;
                current_thread->now_fun = 1;
                current_thread->now_exc = current_thread->top;
                current_thread->top->fun(current_thread->top->arg); //execute task function
                current_thread->now_fun = -1;
                //printf("task done\n");
                task_exit();
                //printf("task exit\n");
            }
            
        }
    }
    if (current_thread->buf_set == 1) {
        longjmp(current_thread->env, 1);
    }
    else {
        current_thread->buf_set = 1;
        if (setjmp(current_thread->env) == 0){
            current_thread->env->sp = (unsigned long)current_thread->stack_p;
            longjmp(current_thread->env, 1);
        }
        else{
            //current_thread->flag = 1;
            current_thread->now_fun = 0;
            //printf("thread fun start %d\n", current_thread->now_fun);
            current_thread->fp(current_thread->arg);
            //printf("thread fun end\n");
            current_thread->now_fun = -1;
            current_thread->flag = 1;
            thread_exit();// Execute the thread's main function.
        }
    }
}

void schedule(void){
    // TODO
    current_thread = current_thread->next;
}
void task_exit(void){
    current_thread->task_cnt--;
    struct task *before = current_thread->top->previous;
    //struct task *now = current_thread->top;
    if (before != NULL){
       
        current_thread->top = before;
        before->next = NULL;
        //free(now->stack_task);
        //free(now);
        dispatch();
    }
    else{
        //free(now->stack_task);
        //free(now);
        current_thread->top = NULL;
    }
}
void thread_exit(void){
    // TODO
    if (current_thread->next != current_thread){ //check if last thread exits
        struct thread *tmp_before = current_thread->previous;
        struct thread *tmp_after = current_thread->next;
        tmp_before->next = tmp_after;
        tmp_after->previous = tmp_before;
        struct thread *tmp = current_thread;
        free(tmp->stack);
        free(tmp);
        current_thread = tmp_after;
        dispatch();
    }
    //}
    else{ //last thread exits
        // TODO
        // Hint: No more thread to execute
        free(current_thread->stack);
        free(current_thread);
        //should return to main
        //in main => end after thread_start_threading
        longjmp(env_st, 1);
    }
}
void thread_start_threading(void){
    // TODO
    if (setjmp(env_st) == 1) {
        return; // return to main and then exits
    }
    schedule();
    dispatch();
}

// part 2
void thread_assign_task(struct thread *t, void (*f)(void *), void *arg){
    // TODO
    struct task *tmp = (struct task*) malloc(sizeof(struct task*));
    
    unsigned long new_stack_task = (unsigned long) malloc(sizeof(unsigned long)*0x100);
    unsigned long new_stack_task_p = new_stack_task + 0x100*8-0x2*8;
    
    tmp->fun = f;
    tmp->arg = arg;
    tmp->task_set = 0;
    tmp->next = NULL;
    tmp->previous = NULL;
    tmp->flag = 0;
    tmp->stack_task = (void *)new_stack_task;
    tmp->stack_task_p = (void *)new_stack_task_p;
    
    /*unsigned long new_stack_task_p = (unsigned long)t->now_stack + sizeof(tmp);
    unsigned long new_t_stack = (unsigned long)t->now_stack + (unsigned long)(sizeof(tmp));
    t->now_stack = (void *)new_t_stack;
    tmp->stack_task_p = (void*)new_stack_task_p;*/
    //t->now_stack += sizeof(tmp);
    
    if (t->top == NULL){
        t->task_cnt++;
        t->top = tmp;
    }
    else{
        t->top->next = tmp;
        tmp->previous = t->top;
        t->top = tmp;
        t->task_cnt++;
    }
}




