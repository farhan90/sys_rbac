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
int idx=0;



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
int write_role(struct role_map *role_map){
	int ret=0;
	char *new_line="\n";	
	FILE *ptr=fopen("role.mp","a+");
	if(!ptr){
		printf("Could not open the file role.mp to save the role\n");
		printf("The errno is %d\n",errno);
		return -1;
	}
	ret=fwrite(role_map,sizeof(struct role_map),1,ptr);
	ret=fwrite(new_line,strlen(new_line),1,ptr);
	if(ret<0){
		printf("Error in writing to file role.mp %d\n",errno);
	}
	
	fclose(ptr);
	return ret;
}


void print_role_map(struct role_map *role_map){
	char *role;	
	if(role_map->role==READ){
		role="READ";
	}
	if(role_map->role==RDWR){
		role="RDWR";
	}
	if(role_map->role==CRTDEL){
		role="CRTDEL";
	}
	printf("%s:%s\n",role_map->name,role);
	
}
void print_role_list(){
	read_role();
	int i=0;
	for(i=0;i<idx;i++){
		print_role_map(&role_list[i]);
	}
	printf("\n");	
}

int read_role(){
	int ret=0;
	char *line=NULL;
	size_t len=0;
	FILE *ptr=fopen("role.mp","r");
	
	if(!ptr){
		printf("Could not open the file role.mp to save the role\n");
		printf("The errno is %d\n",errno);
		return -1;
	}
	while((ret=getline(&line,&len,ptr))!=-1){
		//printf("Retrived line of length %zu :\n",ret);
		struct role_map role_map;
		memcpy(&role_map,line,sizeof(struct role_map));
		role_list[idx]=role_map;
		idx++;	
		//print_role_map((struct role_map*)line);
	}
	
	free(line);
	fclose(ptr);
	return ret;

}

void create_role(){
	char role_name[MAX_CHAR_SIZE];
	int role;
	struct role_map role_map;
	printf("Enter the role name:");
	scanf("%s",&role_name);
	printf("Please select one of the tasks to be assigned to the role\n");
	printf("1)READ - Can only read files\n");
	printf("2)RDWR - Can only read and write files\n");
	printf("3)CRTDEL - Can read, write, create and delete files\n");
	printf("Enter the task number:");
	scanf("%d",&role);
	strncpy(role_map.name,role_name,MAX_CHAR_SIZE);
	role_map.role=role;
	printf("You created the role %s with task %d\n\n",role_map.name,
			role_map.role);
	write_role(&role_map);
		
}

void process_opt(int opt){
	
	if(opt==4){
		printf("Goodbye!\n");
		exit(0);
	}
	if(opt==1){
		assign_role();
	}
	if(opt==2){
		create_role();
	}
	if(opt==3){
		print_role_list();
	}
}

int main(){	
	printf("Welcome to user admin\n");
	while(1){
		int opt=0;
		printf("Please select: \n");
		printf("1)Assign Role\n");
		printf("2)Create Role\n");
		printf("3)List all created roles\n");
		printf("4)Quit\n");
		printf("Enter:");
		scanf("%d",&opt);
		process_opt(opt);

	}	
}





