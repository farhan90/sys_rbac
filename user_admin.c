#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>


#define __NR_rbac 349

int main(){

	int rc;
	char *str="Hello\n";
	rc=syscall(__NR_rbac,(void *)str);
	if(rc==0)
		printf("syscall executed successfully\n");
	else
		printf("syscall returned error %d\n",errno);

	return 0;

}

