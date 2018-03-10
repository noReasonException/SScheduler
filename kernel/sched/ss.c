#include "sched.h"
#include "ss_debug.h"
#include "ss.h"

int debug_stop=1;  //for debug purposes , stops the ss_scheduler //TODO remove for version 1.0

extern void resched_curr(struct rq *rq); //forward declaration
/*[t]*/extern struct ss_task * 	find_ss_task		(struct ss_rq *,struct task_struct *);
/*[t]*/extern int		ss_utill_task_is_dead	(struct task_struct *p);
/*[t]*/extern struct ss_task * 	get_earliest_ss_task	(struct ss_rq*);
/*[t]*/extern int		insert_ss_task_rb_tree 	(struct ss_rq*,struct ss_task*,int flags);
/*[t]*/extern int 		remove_ss_task_rb_tree	(struct ss_rq*,struct ss_task*);
/*[c]*/extern int 		remove_ss_task_rq_list	(struct ss_rq*ss_rq,struct ss_task*ss_task);
/*[c]*/extern int 		insert_ss_task_rq_list	(struct ss_rq*ss_rq,struct task_struct*ss_task);
/*
main scheduling class for Stefs EDF RT Scheduler
by noReasonException
version 0.0.1
last edited 7/2/2018
+--------------------------------------------------------------------------------+
Notes -> to 0.0.1 final
*)Dont forget to implement...
	Name			Description
[*]	find_ss_task 		(check enqueue_task_ss)
[*]]	insert_ss_task_rb_tree	(check again , the enqueue_task_ss)
[t]	remove_ss_task_rb_tree 	(this time check dequeue_task)
[t]	ss_utill_task_is_dead	(must remove from linked list?(state ZOMBIE||DEAD),see dequeue_task)
[n]	remove_ss_task_list	(remome ss_task from linked list)
[*]	get_earliest_ss_task	(Check the check_preempt_curr)
*)Some conversions...
[n] :Function non implemented
[c] :Currently Working on...
[t] :Completed but not tested
[*] :Completed
*/


/*Forward Declarations...*/
static void 	/**/		enqueue_task_ss		(struct rq *rq,struct task_struct *p,int wakeup);
static void 	/**/		dequeue_task_ss		(struct rq *rq,struct task_struct *p,int sleep);
static void 	/**/		check_preempt_curr_ss	(struct rq *rq,struct task_struct *task,int flags);
static struct	task_struct*	pick_next_task_ss	(struct rq *rq,struct task_struct *prev);
static void	/**/		task_tick_ss		(struct rq *rq,struct task_struct *p,int queued);
static void	/**/		put_prev_task_ss	(struct rq *rq,struct task_struct *p);
static void 	/**/		yield_task_ss		(struct rq *rq);
const struct sched_class ss_sched_class={
/*[*]*/	.next			= &dl_sched_class,	//has the next scheduling module , the real time linux scheduler
/*[*]*/	.enqueue_task		= enqueue_task_ss,	//called when a new ss task changes status to TASK_RUNNING
/*[t]*/	.dequeue_task		= dequeue_task_ss,	//called when a ss RUNNING task blocks
/*[t]*/	.check_preempt_curr	= check_preempt_curr_ss,//called to check if the newrly created task must preempt the current one...
/*[*]*/	.pick_next_task		= pick_next_task_ss,	//chooses the most appropiate next ss task to run!
/*[t]*/	.task_tick		= task_tick_ss	,	//every ms the task_tick_ss subtracts 1 from absolute deadline!
/*[*]*/	.put_prev_task		= put_prev_task_ss,
/*[c]*/	.yield_task		= yield_task_ss,	// when a task wants to exit from cpu
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
		if((t=find_ss_task(&rq->ss_rq,p))){
			//t->absolute_deadline=sched_clock()+p->deadline;		//update new deadline!(Commented for debug TODO Remove)
			if(!atomic_read(&rq->ss_rq.nr_running)){				//in case that this enqueue will be the first..
				insert_ss_task_rb_tree(&rq->ss_rq,t,SS_NOFLAG);         //add to red black tree , but not rebalance it!
			}
			else{
				insert_ss_task_rb_tree(&rq->ss_rq,t,SS_ALLOW_RECOLOR);  //add to red black tree(+ rebalance)
			}
			atomic_inc(&rq->ss_rq.nr_running);				//add one to current ss running processes!
		}
	}
	ss_debug("enqueue_task_ss on rq %px in task %px completed , get earliest task return %px",rq,p,get_earliest_ss_task(&rq->ss_rq));

}
SS_EXPORT_IF_DEBUG(enqueue_task_ss);
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
		if((t=find_ss_task(&rq->ss_rq,p))){
			remove_ss_task_rb_tree(&rq->ss_rq,t);
			atomic_dec(&rq->ss_rq.nr_running);
			/*if(ss_utill_task_is_dead(p)){
				remove_ss_task_rq_list(&rq->ss_rq,t); //TODO:Dont forget to implement
			}*/
		}
	}
	ss_debug("dequeue_task_ss invoked on rq:%pK and task_struct :%px",rq,p);
}
SS_EXPORT_IF_DEBUG(dequeue_task_ss);
/*check_preempt_curr_ss
@version 0.0.3

@brief is called when we must check if it is nessesary to preempt the current running task
@param struct rq* rq , The current runqueue
@param struct task   ,the task witch became runnable

Revisions
version_before	version_after		brief			solution		status
0.0.1		0.0.2			compatibility issue	add a int flags		Fixed
								parameter (introduced
								in version 2.6.32.71)
0.0.2		0.0.3			compatibility issue	replace resched_task	Fixed
								with resched_curr
								(introduced to patch
								4443611 v3.16.x)
0.0.3		0.0.4			readabillity fix	replace  *task		TODO
								with  *p so to be
								the same as other
								routines
*/
static void check_preempt_curr_ss (struct rq *rq,struct task_struct *task,int flags ){
	struct ss_task *earl_task	=NULL; //The leftmost node of red-black tree(a.k.a earliest deadline)
	struct ss_task *curr_task	=NULL; //The Current running task
	if(atomic_read(&rq->ss_rq.nr_running)	//if there exist at least one task in runqueue
		&&rq->curr->policy!=SCHED_SS){//and the current task is not a SS task
		resched_curr(rq);
	}
	else{ //in other case , just pick the task with earliest deadline (Leftmost of red-black tree)
		if((earl_task=get_earliest_ss_task(&rq->ss_rq))){          //get earliest
			if((curr_task=find_ss_task(&rq->ss_rq,rq->curr))){ //get current
				if((earl_task->absolute_deadline < curr_task->absolute_deadline)){ //compare!
					resched_curr(rq);
				}
			}
		}
	}
	ss_debug("check_preempt_task_ss invoked on rq:%px and task_struct :%px",rq,task);

}
SS_EXPORT_IF_DEBUG(check_preempt_curr_ss);
/*pick_next_task_ss
@version 0.0.2
@brief Returns the most appropiate task to run (this one with the earliest absolute deadline)
@param struct rq *rq , The current runqueue
Revisions
version_before  version_after           brief                   solution                status
0.0.1  	        0.0.2                   compatibility issue    	task_struct *ptr in	Fixed
								param list (introduced
                                                                in 3.15-rc1)
*/
static struct task_struct *pick_next_task_ss(struct rq *rq,struct task_struct *prev){
	struct ss_task *retval = get_earliest_ss_task(&rq->ss_rq);
	if(!retval || !debug_stop){
		return NULL;
	}
	ss_debug("pick_next_task_ss returns ss_task in %px",retval);
	return retval->task;
}
SS_EXPORT_IF_DEBUG(pick_next_task_ss);
/*
task_tick_ss(struct rq*,struct task_struct*,int queued)
	ticks in every timer interrupt , removes 1 from relative deadline of the task
@param rq 	 the current runqueue
@param p  	 the current running struct
@param queued	 1 in case of TASK_ON_RQ_QUEUED
*/
static void task_tick_ss(struct rq*rq,struct task_struct*p,int queued){
	p->deadline-=1;
	if(p->deadline<00){
		set_tsk_need_resched(p);
		debug_stop=0;
		ss_debug("task (%px) on rq(%px) stopped by deadline",p,rq);
	}
}
SS_EXPORT_IF_DEBUG(task_tick_ss);

/*EOPNOSUPP*/
static void put_prev_task_ss(struct rq*rq,struct task_struct*p){
	return;

}
SS_EXPORT_IF_DEBUG(put_prev_task_ss);
/**
[c] yield_task_ss(struct rq*rq)
Called when a ss_task wants voluntarily to preempt by another task
@param struct rq *rq , the current runqueue
*/
static void yield_task_ss (struct rq *rq){
	ss_debug("yield_task_ss requested on rq(%px)",rq);
	resched_curr(rq);
	debug_stop=0; //for develepoment purposes
}
SS_EXPORT_IF_DEBUG(yield_task_ss);
