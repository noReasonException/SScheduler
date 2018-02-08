#include "sched.h" //to reference struct ss_rq on that file
#include <lib/rbtree.h>
/*

*)static void init_ss_rq(struct ss_rq**)
*)initialize a ss runqueue struct , for each possible cpu
*)is called by sched_init procedure on file core.c
@param struct ss_rq**ss_rq , the struct to initialize
[t]				No tested
*/

static void init_ss_rq(struct ss_rq * ss_rq){
	ss_rq->ss_root=RB_ROOT;			//initialize red-black self-balanced tree
	atomic_set(&ss_rq->nr_running,0);	//set the current running process counter to zero
	INIT_LIST_HEAD(&ss_rq->ss_list);	//initialize the circular doubly linked list(for RUNNABLE TASKS)
}
