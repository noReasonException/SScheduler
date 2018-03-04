
#include <linux/rbtree.h>
#include <asm/atomic.h>
#include "ss_debug.h"
#include "sched.h"
#include "ss.h"
extern int ss_prio(int prio);
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
/*
extern void init_ss_sched_attr(struct sched_attr *attr,const struct sched_param *param)
initializes the fields of sched_attr to pass parameters in sscheduler
called by _sched_setscheduler(struct task_struct *p) in /kernel/sched/core.c
TODO:ss_id maybe? :P
*/

extern void init_ss_sched_attr(struct sched_attr*attr,const struct sched_param*param){
	attr->deadline=PRIO_TO_SS_DEADLINE(param->sched_priority);
	ss_debug("init_ss_sched_attr initialized struct on %px with deadline %d ",attr,attr->deadline);

}
SS_EXPORT_IF_DEBUG(init_ss_sched_attr);
/*
extern struct ss_task *alloc_ss_task(struct task_struct *p)
Allocates a ss_task and initialized properly
@Note , you must manually call list_add to add it in your runqueue!
@param p , the task_struct whitch ss_task->task will point to
@Note , the @param p is not checked , if you pass invalid pointer , then...Oops! , or maybe PANIC(not at the disco :P)
@Nullable*/
extern struct ss_task* alloc_ss_task(struct task_struct *p){
	struct ss_task *retval;
	if(!(retval=kmalloc(sizeof(struct ss_task),GFP_KERNEL))){
		return NULL;
	}
//	retval->ss_node=RB_ROOT;
	retval->absolute_deadline=0;
	INIT_LIST_HEAD(&retval->ss_list_node);
	retval->task=p;
	ss_debug("alloc_ss_task created task on %px",p);
	return retval;
}
SS_EXPORT_IF_DEBUG(alloc_ss_task);
