// vim: noet:sts=8:ts=8:sw=8
#include <stdio.h>
#include <sys/syscall.h>
#include <stdlib.h>


#define __NR_get_child_pid 359

void print_list(pid_t* list, size_t limit) {
	int i=0;
	for (; i<limit; i++) {
		printf("list content: index %d value %d \n",i,list[i]);
	}
}

int main () {
	long res;
	int child_pid = fork();
	child_pid = fork();
	child_pid = fork();
	if (child_pid < 0 )
	    printf("Fork failed %i \n", child_pid);
	else if (child_pid > 0) {
		size_t limit = 2;
		size_t nr_children;
		pid_t pid_list[limit]; // to store our results


		// CASE : Arbitrary address for num_children
		res = syscall(__NR_get_child_pid, pid_list, limit, (size_t*)47424742);
		printf ( "Testing arbitrary address for num_children. Syscall returned %d \n", res);

		// CASE : NULL pid_list, non initialized
		res = syscall(__NR_get_child_pid, NULL, limit, &nr_children); 
		printf ( "Testing NULL address for pids_list. Syscall returned %d \n", res);

		// CASE : Normal execution, num_children < limit
		res= syscall(__NR_get_child_pid,pid_list, limit, &nr_children);
		printf("Testing Nr_children = 3, limit = %d. Syscall returned %d , nr_children is %d\n", limit, res, nr_children);
		printf("LIST OF CHILDREN PIDs syscall\n");
		print_list(pid_list, (nr_children <= limit) ? nr_children : limit);
	}
	
	return 0;  
}
