#include "sched.h"
//*
main scheduling class for Stefs EDF RT Scheduler
by noReasonException
version 0.0.1
last edited 6/2/2018
+--------------------------------------------------------------------------------+
Notes -> to 0.0.1 final
dont forget to implement...
	find_ss_task 		(check enqueue_task_ss)
	insert_ss_task_rb_tree	(check again , the enqueue_task_ss)
	remove_ss_task_rb_tree 	(this time check dequeue_task)
	ss_utill_task_is_dead	(must remove from linked list?(state ZOMBIE||DEAD),see dequeue_task)
	remome_ss_task_list	(remome ss_task from linked list)
*//

const struct sched_class ss_sched_class={
	.next			= &rt_sched_class,	//has the next scheduling module , the real time linux scheduler
	.enqueue_task		= enqueue_task_ss,	//called when a new ss task changes status to TASK_RUNNING
	.dequeue_task		= dequeue_task_ss,	//called when a ss RUNNING task blocks
	.check_preempt_curr	= check_preempt_curr_ss,//called to check if the newrly created task must preempt the current one...
	.pick_next_task		= pick_next_task_ss,	//chooses the most appropiate next ss task to run!
};
//*
enqueue_task_ss	procedure , is called by linux scheduler's class system when a ss task want to became TASK_RUNNABLE
@param struct rq *rq 		the per-processor run queue
@param struct task_struct *p	the task_struct who wants to became TASK_RUNNABLE
@param int wakeup


*//
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
//*
dequeue_task_ss	procedure , is called by linux scheduler's class system when a ss task want to quit from TASK_RUNNABLE state
@note basically this function undo whatever enqueue_task_ss does :P
@param struct rq *rq            the per-processor run queue
@param struct task_struct *p    the task_struct who wants to exit from runqueue
@param int sleep
*//
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
