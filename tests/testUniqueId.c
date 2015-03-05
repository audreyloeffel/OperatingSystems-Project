#include <sys/syscall.h>
#include <stdio.h>

#define __NR_get_unique_id 358

int main(void) {
	
    int uuid;
    int uuid2;
	int res;
	
	printf("First call\n"); fflush(stdout);
	res = syscall(__NR_get_unique_id, (void *) 0);
	printf("Syscall returned %d, uuid is %d\n", res, uuid);

	printf("Second call\n"); fflush(stdout);
	res = syscall(__NR_get_unique_id, &uuid);
	printf("Syscall returned %d, uuid is %d\n", res, uuid);


	printf("Third call\n"); fflush(stdout);
	res = syscall(__NR_get_unique_id, &uuid2);
	printf("Syscall returned %d, uuid is %d\n", res, uuid2);


	return 0;
}
