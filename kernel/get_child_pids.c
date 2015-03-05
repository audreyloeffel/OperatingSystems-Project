#include <linux/linkage.h>
#include <linux/uaccess.h>
#include <linux/spinlock.h>
#include <linux/sched.h>


asmlinkage long get_child_pids(pid_t* list, size_t limit,
		size_t* num_children) {

	struct task_struct* task = NULL;
	long nb_children = 0;
	long ret;

	read_lock (&tasklist_lock);

	list_for_each_entry(task, &current->children, sibling)
	{
		nb_children++;

		if (nb_children <= limit && limit != 0) {
			*list = task->pid;
			list++; //next elem in the list
		}
	}
	read_unlock(&tasklist_lock); //release lock before put_user because put_user can sleep -> avoid blocking	

	ret = put_user(nb_children, num_children); //(value, ptr)

	if (ret != 0) {
		return ret;  // put_user return -EFAULT on error
	}
	if (nb_children > limit) {
		ret = -ENOBUFS;
	} else if (limit = 0) {
		*list = NULL;
	}
	return ret;
}
