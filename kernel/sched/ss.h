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
