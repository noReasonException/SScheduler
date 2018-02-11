#include <linux/rbtree.h>
#include "sched.h"
/*
ss_utill_task_is_dead(struct ss_task *ss_task)
returns 1 if task must remove from linked list of tasks , 0 if not
@param ss_task the ss_task of the task to examine  @see sched.h/struct ss_task
*/
extern int ss_utill_task_is_dead(struct ss_task *ss_task){
	return ss_task->task->state==TASK_DEAD||
		ss_task->task->state==EXIT_DEAD||
		ss_task->task->state==EXIT_ZOMBIE;


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
extern struct ss_task *find_ss_task(struct ss_rq *ss_rq,struct task_struct *p){
	struct list_head*htmp;			//temponary variable to each list node
	struct ss_task  *tmp; 			//temponary var , to iterate over each element
	list_for_each(htmp,&ss_rq->ss_list) {
		tmp=list_entry(htmp,struct ss_task,ss_list_node);
		if(tmp->task==p)return tmp;
	}
	return NULL;

}
/*
struct ss_task *get_earliest_ss_task(struct ss_rq*)
[t]
@brief picks the leftmost node(with earliest deadline ;) )
@param struct ss_rq , the currenly runqueue
*/
extern struct ss_task *get_earliest_ss_task(struct ss_rq*ss_rq){
	struct rb_root* temp_iterate  = &ss_rq->ss_root;
	struct rb_node* temp_container=NULL;
	while(temp_iterate->rb_left) {				//you must remember , in rb_tree , all leaf nodes does not
		temp_iterate=temp_iterate->rb_left;		//contain data
	}
	return (struct ss_task)container_of(temp_iterate,struct ss_task,ss_node); //return the leftmode ss_struct pointer

}
/*
int insert_ss_task_rb_tree(struct ss_rq*,struct ss_task*)
@brief Imports a new task into sscheduler's red-black tree!
@param struct ss_rq  * ss_rq   -> The current runqueue
@param struct ss_task* ss_task -> The process to insert in rbtree!
@returns >1 if all okay , 0 otherwise*/
extern int insert_ss_task_rb_tree (struct ss_rq*ss_rq,struct ss_task*ss_task){
	struct rb_node * temp  = &(ss_rq->ss_root.rb_node);	//At first , temp node starts from root node!
	struct rb_node * parent= NULL;				//..and of course , has no parent :P
	struct ss_task * temp_container=NULL;			//temp's container
	while(temp) {						//while temp variable isnt NULL (0)
		temp_container=container_of(*temp,struct ss_task,ss_node);
		parent=temp;
		if(ss_task->absolute_deadline< temp_container->absolute_deadline){
			temp=temp->rb_left;
		}
		else if(ss_task->absolute_deadline > temp_container->absolute_deadline){
			temp=temp->rb_right;
		}
		else{
			printk(KERN_ERR"insert_ss_task_rb_tree: fail due to deadline confict! ");
			return 0;
		}
	}
	rb_link_node(&ss_task->ss_node,parent,temp);		//insert into rbtree
	rb_insert_color(&ss_task->ss_node,&ss_rq->ss_root);	//rebalance if nessesary
	return 1;
}

/*
int remove_ss_task_rb_tree(struct ss_rq*ss_rq,struct ss_task *ss_task)
@brief removes a task from red-black tree :O
@param struct ss_rq  *ss_rq , the current runqueue
@param struct ss_task*ss_task ,the process to remove from red-black tree
@return 1 on success , 0 otherwise
*/
extern int remove_ss_task_rb_tree(struct ss_rq*ss_rq,struct ss_task*ss_task){
	rb_erase(&ss_task->ss_node,ss_rq->ss_root.rb_node);
	return 1;

}
