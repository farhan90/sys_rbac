
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>

struct policy{
	uid_t uid;
	int role;	
};

int main(){

	struct passwd *res;
	struct policy pol;

	FILE *ptr_file=fopen("/proc/my_test","w");


	if(!ptr_file){
		printf("Could not open the file /proc/my_test %d\n",errno);
		exit(1);
	} 
	errno=0;
	res=getpwnam("farhan");

	if(res==NULL){
		printf("Could not get the user name, errno %d\n",errno);
		goto out;
		exit(1);
	}

	pol.uid=res->pw_uid;
	pol.role=2;

	printf("The user id is %d\n",res->pw_uid);
	int ret;
	ret=fwrite(&pol,sizeof(struct policy),1,ptr_file);
	if(ret<0){
		printf("Could not write the file errno %d\n",errno);
	}

out:
	fclose(ptr_file);

	return 0;
}





