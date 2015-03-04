#include <linux/linkage.h>
#include <linux/module.h>  /* Needed by all modules */
#include <linux/kernel.h>  /* Needed for KERN_ALERT */
#include <asm/atomic.h>
#include <linux/uaccess.h>

#ifndef ATOMIC_VALUE 
	#define ATOMIC_VALUE 1 
	atomic_t v = ATOMIC_INIT(1);
#endif

asmlinkage long sys_get_unique_id(int *uuid)
{
	int ret = -EFAULT;
	if (uuid != (void *) 0){
		atomic_inc(&v);
		ret = put_user(atomic_read(&v), uuid);
		//uuid is the destination address, in user space
		//atomic_read(&v) is the value to copy to user_space
		//It copies a single value from kernel space to user_space
		//Returns zero on success, or -EFAULT on error. 
	}
	
	return ret;	
}

