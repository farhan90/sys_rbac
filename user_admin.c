
#include <stdlib.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>

#define MAX_CHAR_SIZE 20
#define MAX_ROLE 20
#define READ 1
#define RDWR 2
#define CRTDEL 3

struct policy{
	uid_t uid;
	int role;	
};

struct role_map{
	char name [MAX_CHAR_SIZE];
	int role;
};

struct role_map role_list[MAX_ROLE];

int write_policy(char *name,int role){
	struct passwd *res;
	struct policy pol;
	int ret=0;

	FILE *ptr_file=fopen("/proc/my_test","w");


	if(!ptr_file){
		printf("Could not open the file /proc/my_test %d\n",errno);
		ret=-1;
	} 
	errno=0;
	res=getpwnam(name);

	if(res==NULL){
		printf("Could not get the user name\n");
		ret=-1;
		goto out;
		
	}

	pol.uid=res->pw_uid;
	pol.role=role;

	printf("The user id is %d and role is %d\n",res->pw_uid,pol.role);
	ret=fwrite(&pol,sizeof(struct policy),1,ptr_file);
	if(ret<0){
		printf("Could not write the file errno %d\n",errno);
	}

out:
	fclose(ptr_file);

	return ret;
}

void assign_role(){
	char name[MAX_CHAR_SIZE];
	int role;
	printf("Enter user name:");
	scanf("%s",&name);
	printf("Enter role:");
	scanf("%d",&role);
	write_policy(name,role);

}

void create_role(){
	char role_name[MAX_CHAR_SIZE];
	int role;
	printf("Enter the role name:");
	scanf("%s",&role_name);
	printf("Please select one of the tasks to be assigned to the role\n");
	printf("1)READ - Can only read files\n");
	printf("2)RDWR - Can only read and write files\n");
	printf("3)CRTDEL - Can read, write, create and delete files\n");
	printf("Enter the task number:");
	scanf("%d",&role);
}

void process_opt(int opt){
	
	if(opt==3){
		printf("Goodbye!\n");
		exit(0);
	}
	if(opt==1){
		assign_role();
	}
	if(opt==2){
		create_role();
	}
}

int main(){
	printf("Welcome to user admin\n");
	while(1){
		int opt=0;
		printf("Please select: \n");
		printf("1)Assign Role\n");
		printf("2)Create Role\n");
		printf("3)Quit\n");
		printf("Enter:");
		scanf("%d",&opt);
		process_opt(opt);

	}	
}





