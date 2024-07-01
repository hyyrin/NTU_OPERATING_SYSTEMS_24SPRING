#include "kernel/types.h"
#include "user/user.h"
#include "user/list.h"
#include "user/threads.h"
#include "user/threads_sched.h"

#define NULL 0

/* default scheduling algorithm */
struct threads_sched_result schedule_default(struct threads_sched_args args)
{
    struct thread *thread_with_smallest_id = NULL;
    struct thread *th = NULL;
    list_for_each_entry(th, args.run_queue, thread_list) {
        if (thread_with_smallest_id == NULL || th->ID < thread_with_smallest_id->ID)
            thread_with_smallest_id = th;
    }

    struct threads_sched_result r;
    if (thread_with_smallest_id != NULL) {
        r.scheduled_thread_list_member = &thread_with_smallest_id->thread_list;
        r.allocated_time = thread_with_smallest_id->remaining_time;
    } else {
        r.scheduled_thread_list_member = args.run_queue;
        r.allocated_time = 1;
    }

    return r;
}

/* MP3 Part 1 - Non-Real-Time Scheduling */
/* Weighted-Round-Robin Scheduling */
struct threads_sched_result schedule_wrr(struct threads_sched_args args)
{
    //struct threads_sched_result r;
    // TODO: implement the weighted round-robin scheduling algorithm

    //return r;
    int curr_time = args.current_time;
    int time_quantum = args.time_quantum;
    struct release_queue_entry *the = NULL;
       struct thread *thread_with_smallest_id = NULL;
       struct thread *th = NULL;
       list_for_each_entry(th, args.run_queue, thread_list) {
           if (thread_with_smallest_id == NULL)
               thread_with_smallest_id = th;
       }

       struct threads_sched_result r;
       int time_slice = time_quantum * thread_with_smallest_id->weight;
       if (thread_with_smallest_id != NULL) {
           r.scheduled_thread_list_member = &thread_with_smallest_id->thread_list;
           if (thread_with_smallest_id->processing_time >= time_slice){
               r.allocated_time = time_slice;
               thread_with_smallest_id->processing_time -= time_slice;
           }
           else{
               r.allocated_time = thread_with_smallest_id->processing_time;
               thread_with_smallest_id->processing_time = 0;
           }
           
           //r.allocated_time = thread_with_smallest_id->remaining_time;
       }
       else {
           r.scheduled_thread_list_member = args.run_queue;
                          int min = 10000;
                          list_for_each_entry(the, args.release_queue, thread_list){
                           if (the->release_time < min) min = the->release_time;
                          }
                          r.allocated_time = min - curr_time;
       }
       return r;
    
}

/* Shortest-Job-First Scheduling */
struct threads_sched_result schedule_sjf(struct threads_sched_args args)
{
   
    // TODO: implement the shortest-job-first scheduling algorithm
    struct release_queue_entry *the = NULL;
    int curr_time = args.current_time;
        struct threads_sched_result r;
        struct release_queue_entry *th = NULL;
        struct thread *tmp = NULL;
        struct thread *now_thread = NULL;
        int burst_time = 1000;
        list_for_each_entry(tmp, args.run_queue, thread_list){
            if (tmp->processing_time < burst_time){
                now_thread = tmp;
                burst_time = tmp->processing_time;
            }
            else if (tmp->processing_time == burst_time && tmp->ID < now_thread->ID) now_thread = tmp;
        }
        int min_time = 101;
        int max_right = curr_time + now_thread->processing_time;
        list_for_each_entry(th, args.release_queue, thread_list) {
            if (max_right > th->release_time && max_right - th->release_time >= th->thrd->processing_time && th->release_time < min_time){
                min_time = th->release_time;
            }
        }
        if (now_thread != NULL){
            r.scheduled_thread_list_member = &now_thread->thread_list;
            if (min_time != 101){
                r.allocated_time = min_time - curr_time;
                now_thread->processing_time -= r.allocated_time;
            }
            else{
                r.allocated_time = now_thread->processing_time;
                now_thread->processing_time = 0;
            }
        }
        else {
            r.scheduled_thread_list_member = args.run_queue;
                           int min = 10000;
                           list_for_each_entry(the, args.release_queue, thread_list){
                            if (the->release_time < min) min = the->release_time;
                           }
                           r.allocated_time = min - curr_time;
        }
        return r;

    
}
/* MP3 Part 2 - Real-Time Scheduling*/
/* Least-Slack-Time Scheduling */
// slack time = current_deadline - current_time - remaining_time;
struct threads_sched_result schedule_lst(struct threads_sched_args args)
{
    int curr_time = args.current_time;
    struct threads_sched_result r;
    struct release_queue_entry *th = NULL;
    struct thread *tmp = NULL;
    struct thread *now_thread = NULL;
    struct thread *dead_thread = NULL;
    int min_slack = 10000;
    list_for_each_entry(tmp, args.run_queue, thread_list){
        if (tmp->current_deadline - curr_time - tmp->remaining_time < min_slack){
            min_slack = tmp->current_deadline - curr_time - tmp->remaining_time;
            now_thread = tmp;
        }
        else if (tmp->current_deadline - curr_time - tmp->remaining_time == min_slack && tmp->ID < now_thread->ID) now_thread = tmp;
    }
    // miss deadline process
    list_for_each_entry(tmp, args.run_queue, thread_list){
        if (tmp->remaining_time > 0 && curr_time >= tmp->current_deadline) {
            if (dead_thread == NULL) dead_thread = tmp;
            else if (dead_thread != NULL && tmp->ID < dead_thread->ID) dead_thread = tmp;
        }
    }
    
    int max_right = 10000;
    list_for_each_entry(th, args.release_queue, thread_list){
        int now_curr = th->release_time;
        int new_slack = th->release_time + th->thrd->period - th->release_time - th->thrd->remaining_time;
        int old_slack = now_thread->current_deadline - now_curr - now_thread->remaining_time + now_curr - curr_time;
        if (new_slack <= old_slack && now_curr < max_right) max_right = now_curr;
    }
    
    if (now_thread != NULL){
        if (max_right != 10000){
            if (max_right > now_thread->current_deadline){
                if (dead_thread == NULL) dead_thread = now_thread;
                else if (dead_thread != NULL && now_thread->ID < dead_thread->ID) dead_thread = now_thread;
            }
        }
        else{
            if (curr_time + now_thread->remaining_time > now_thread->current_deadline){
                if (dead_thread == NULL) dead_thread = now_thread;
                else if (dead_thread != NULL && now_thread->ID < dead_thread->ID) dead_thread = now_thread;
            }
        }
    }

    if (now_thread != NULL && dead_thread == NULL){
            r.scheduled_thread_list_member = &now_thread->thread_list;
            if (max_right != 10000){
                r.allocated_time = max_right - curr_time;
            }
            else{
                r.allocated_time = now_thread->remaining_time;
                now_thread->remaining_time = 0;
            }
        }
        else if (now_thread == NULL && dead_thread == NULL){
            r.scheduled_thread_list_member = args.run_queue;
                           int min = 10000;
                           list_for_each_entry(th, args.release_queue, thread_list){
                            if (th->release_time < min) min = th->release_time;
                           }
                           r.allocated_time = min - curr_time;
        }
        else if (dead_thread != NULL){
            r.scheduled_thread_list_member = &dead_thread->thread_list;
            r.allocated_time = 0;
        }
        return r;

}
/* Deadline-Monotonic Scheduling */
struct threads_sched_result schedule_dm(struct threads_sched_args args)
{
   
    // TODO: implement the deadline-monotonic scheduling algorithm
    int curr_time = args.current_time;
    struct threads_sched_result r;
    struct release_queue_entry *th = NULL;
    struct thread *tmp = NULL;
    struct thread *now_thread = NULL;
    struct thread *dead_thread = NULL;
    int min_deadline = 10000;
    list_for_each_entry(tmp, args.run_queue, thread_list){
        if (tmp->deadline < min_deadline){
            now_thread = tmp;
            min_deadline = tmp->deadline;
        }
        else if (tmp->deadline == min_deadline && tmp->ID < now_thread->ID) now_thread = tmp;
    }
    list_for_each_entry(tmp, args.run_queue, thread_list){
        if (tmp->remaining_time > 0 && curr_time >= tmp->current_deadline) {
            if (dead_thread == NULL) dead_thread = tmp;
            else if (dead_thread != NULL && tmp->ID < dead_thread->ID) dead_thread = tmp;
        }
    }
    int min_right = curr_time + now_thread->remaining_time;
    list_for_each_entry(th, args.release_queue, thread_list){
        if (th->thrd->deadline < now_thread->deadline && th->release_time < min_right) min_right = th->release_time;
        else if (th->thrd->deadline == now_thread->deadline && th->release_time < min_right && th->thrd->ID < now_thread->ID) min_right = th->release_time;
    }
    if (now_thread != NULL){
        if (min_right != curr_time + now_thread->remaining_time){
            if (min_right > now_thread->current_deadline){
                if (dead_thread == NULL) dead_thread = now_thread;
                else if (dead_thread != NULL && now_thread->ID < dead_thread->ID) dead_thread = now_thread;
            }
        }
        else{
            if (curr_time + now_thread->remaining_time > now_thread->current_deadline){
                if (dead_thread == NULL) dead_thread = now_thread;
                else if (dead_thread != NULL && now_thread->ID < dead_thread->ID) dead_thread = now_thread;
            }
        }
    }

    if (now_thread != NULL && dead_thread == NULL){
            r.scheduled_thread_list_member = &now_thread->thread_list;
            if (min_right != curr_time + now_thread->remaining_time){
                r.allocated_time = min_right - curr_time;
            }
            else{
                r.allocated_time = now_thread->remaining_time;
                now_thread->remaining_time = 0;
            }
        }
        else if (now_thread == NULL && dead_thread == NULL){
            r.scheduled_thread_list_member = args.run_queue;
                           int min = 10000;
                           list_for_each_entry(th, args.release_queue, thread_list){
                            if (th->release_time < min) min = th->release_time;
                           }
                           r.allocated_time = min - curr_time;
        }
        else if (dead_thread != NULL){
            r.scheduled_thread_list_member = &dead_thread->thread_list;
            r.allocated_time = 0;
        }
    return r;
}










