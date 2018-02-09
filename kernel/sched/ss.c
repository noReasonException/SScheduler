#include "sched.h"

/*[t]*/struct ss_task * find_ss_task		(struct ss_rq *,struct task_struct *);
/*[t]*/int		ss_utill_task_is_dead	(struct task_struct *p);
/*[t]*/struct ss_task * get_earliest_ss_task	(struct ss_rq*);
/*[c]*/int		insert_ss_task_ rb_tree (struct ss_rq*,struct ss_task*);
/*
main scheduling class for Stefs EDF RT Scheduler
by noReasonException
version 0.0.1
last edited 7/2/2018
+--------------------------------------------------------------------------------+
Notes -> to 0.0.1 final
*)Dont forget to implement...
	Name			Description
[t]	find_ss_task 		(check enqueue_task_ss)
[n]	insert_ss_task_rb_tree	(check again , the enqueue_task_ss)
[n]	remove_ss_task_rb_tree 	(this time check dequeue_task)
[t]	ss_utill_task_is_dead	(must remove from linked list?(state ZOMBIE||DEAD),see dequeue_task)
[n]	remome_ss_task_list	(remome ss_task from linked list)
[t]	get_earliest_ss_task	(Check the check_preempt_curr)
*)Some conversions...
[n] :Function non implemented
[c] :Currently Working on...
[t] :Completed but not tested
[*] :Completed
*/

const struct sched_class ss_sched_class={
/*[*]*/	.next			= &rt_sched_class,	//has the next scheduling module , the real time linux scheduler
/*[t]*/	.enqueue_task		= enqueue_task_ss,	//called when a new ss task changes status to TASK_RUNNING
/*[t]*/	.dequeue_task		= dequeue_task_ss,	//called when a ss RUNNING task blocks
/*[t]*/	.check_preempt_curr	= check_preempt_curr_ss,//called to check if the newrly created task must preempt the current one...
/*[n]*/	.pick_next_task		= pick_next_task_ss,	//chooses the most appropiate next ss task to run!
};

/*
enqueue_task_ss	procedure , is called by linux scheduler's class system when a ss task want to became TASK_RUNNABLE
@param struct rq *rq 		the per-processor run queue
@param struct task_struct *p	the task_struct who wants to became TASK_RUNNABLE
@param int wakeup
*/
static void enqueue_task_ss(struct rq *rq,struct task_struct *p,int wakeup){
	struct ss_task *t=NULL; 		//every task_struct has a ss_task inside
	if(p){
		if((t=find_ss_task(rq,p)){
			t->absolute_deadline=sched_clock()+p->deadline;		//update new deadline!
			insert_ss_task_rb_tree(&rq->ss_rq,t);			//add to red black tree
			atomic_inc(&rq->ss_rq.nr_running);			//add one to current ss running processes!
			printk(KERN_CRIT"ss:new task submitted");		//debug purposes...
		}
	}
}

/*
dequeue_task_ss	procedure
is called by linux scheduler's class system when a ss task want to quit from TASK_RUNNABLE state
@note basically this function undo whatever enqueue_task_ss does :P
@param struct rq *rq            the per-processor run queue
@param struct task_struct *p    the task_struct who wants to exit from runqueue
@param int sleep
*/
static void dequeue_task_ss(struct rq *rq , struct task_struct *p,int sleep)
{
	struct ss_task *t=NULL;
	if(p){
		if(t=find_ss_task(&rq->ss_rq,p)){
			remove_ss_task_rb_tree(&rq->ss_rq,t);
			atomic_dec(&rq->ss_rq.nr_running);
			if(ss_utill_task_is_dead(p)){
				rem_ss_task_list(&rq->ss_rq,t->task); //check again
			}
		}
	}
}
/*check_preempt_curr_ss
is called when we must check if it is nessesary to preempt the current running task
@param struct rq* rq , The current runqueue
@param struct task   ,the task witch became runnable
*/
static void check_preempt_curr_ss (struct rq *rq,struct task_struct *task ){
	struct ss_task *earl_task	=NULL; //The leftmost node of red-black tree(a.k.a earliest deadline)
	struct ss_task *curr_task	=NULL; //The Current running task
	if(atomic_read(&rq->ss_rq.nr_running)	//if there exist at least one task in runqueue
		&&rq->curr->policy!=STEF_SCHED){//and the current task is not a SS task
		resched_task(rq->curr);
	}
	else{ //in other case , just pick the task with earliest deadline (Leftmost of red-black tree)
		if(earl_task=get_earliest_ss_task(&rq->ss_rq)){          //get earliest
			if(curr_task=find_ss_task(&rq->ss_rq,rq->curr)){ //get current
				if(earl_task->absolute_deadline < curr_task->absolute_deadline){ //compare!
					resched_task(rq->curr);
				}
			}
		}
	}
}
/*pick_next_task_ss
Returns the most appropiate task to run (this one with the earliest absolute deadline)
@param struct rq *rq , The current runqueue

*/
static struct task_struct *pick_next_task_ss(struct rq *rq){
	return get_earliest_ss_task(&rq->ss_rq);
}
