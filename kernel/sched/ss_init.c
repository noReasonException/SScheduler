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
