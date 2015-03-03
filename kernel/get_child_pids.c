// PAS FINI 

#include <spinlock.h>

asmlinkage long get_child_pids(pid_t* list, size_t limit, size_t* num_children){

task_struct* task = NULL;
long nb_children = 0;


Read_lock(&tasklist_lock);

	list_for_each_entry(task, &current->children, sibling){
		
		nb_children++;
		
		if(nb_children < limit && limit != 0){
			*list = task->pid;
			list++; //next elem in the list
	}
	
	}
Read_unlock(&tasklist_lock);
// use put_user(num_children);
return 0;
}