#include "sched.h" //to reference struct ss_rq on that file
#include <linux/rbtree.h>
#include <asm/atomic.h>
#include "ss_debug.h"

/*

*)static void init_ss_rq(struct ss_rq**)
*)initialize a ss runqueue struct , for each possible cpu
*)is called by sched_init procedure on file core.c
@param struct ss_rq**ss_rq , the struct to initialize
[t]				No tested
*/
extern void init_ss_rq(struct ss_rq * ss_rq,int cpu_id){
	ss_rq->ss_root=RB_ROOT;			//initialize red-black self-balanced tree
//	ss_debug("rbtree initialized on processor %d",cpu_id);
	atomic_set(&ss_rq->nr_running,0);	//set the current running process counter to zero
//	ss_debug("ss_rq->nr_running initialized to %d",atomic_read(&ss_rq->nr_running));
	INIT_LIST_HEAD(&ss_rq->ss_list);	//initialize the circular doubly linked list(for RUNNABLE TASKS)
	ss_debug("ss runqueue initialized  %d",cpu_id);
}
SS_EXPORT_IF_DEBUG(init_ss_rq);
/**/

extern void init_ss_sched_attr(struct sched_attr*attr){
	attr->deadline=0;
	ss_debug("init_ss_sched_attr initialized struct on %pK",attr);

}
/*
extern struct ss_task *alloc_ss_task(struct task_struct *p)
Allocates a ss_task and initialized properly
@Note , you must manually call list_add to add it in your runqueue!
@param p , the task_struct whitch ss_task->task will point to
@Note , the @param p is not checked , if you pass invalid pointer , then...Oops! , or maybe PANIC(not at the disco :P)*/
extern struct ss_task* alloc_ss_task(struct task_struct *p){
	struct ss_task *retval=kmalloc(sizeof(struct ss_task),GFP_KERNEL);
	retval->ss_node=RB_ROOT;
	retval->absolute_deadline=0;
	INIT_LIST_HEAD(&retval->ss_list_node);
	retval->task=p;
	ss_debug("alloc_ss_task created task on %pK",p);
	return retval;
}
