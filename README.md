# OS-Project

1 - get_unique_pid
We used the atomic counter alternative for this exercice, as to remove the need for locks and make it easier. The rest is pretty much straightforward.

2 - get_child_pids
For this one we had to use spinlocks to ensure the concurrency. There is a loop that lists the children of the current task, while increasing the number of children variable and adding the children pids in a list that is returned (in user space, using put_user, of course) afterwards.
As mentionned in the code, put_user can sleep, so the spinlock has to be freed before that.