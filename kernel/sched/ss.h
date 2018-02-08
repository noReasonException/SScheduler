/*
ss_utill_task_is_dead(struct ss_task *ss_task)
returns 1 if task must remove from linked list of tasks , 0 if not
@param ss_task the ss_task of the task to examine  @see sched.h/struct ss_task
*/
static int ss_utill_task_is_dead(struct ss_task *ss_task){
	return ss_task->task.state==TASK_DEAD||
		ss_task->task.state==EXIT_DEAD||
		ss_task->task.state==EXIT_ZOMBIE;

}
/*
struct ss_task *find_ss_task (struct ss_rq *ss_rq,struct task_struct *p)
@brief Search and return the ss_task struct of the task_struct * given on runqueue given
@note  Just iterate over ss_rq->ss_list(i) , if if(i->task==p) return p , otherwise return NULL (meaning that the
task_struct given has not STEF_SCHED policy! :))
@return an struct ss_task , with ss_task->task==p
	NULL otherwise
@used_in
	*)enqueue_task_ss (ss.c)
	*)dequeue_task_ss (ss.c)

[t]
*/
struct ss_task *find_ss_task(struct ss_rq *ss_rq,struct task_struct *p){
	struct list_head*htmp;			//temponary variable to each list node
	struct ss_task  *tmp; 			//temponary var , to iterate over each element
	list_for_each(htmp,&ss_rq->ss_list) {
		tmp=list_entry(htmp,struct ss_task,ss_list);
		if(tmp->task==p)return tmp;
	}
	return NULL;
}
