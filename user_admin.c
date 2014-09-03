#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>

#define MAX_CHAR_SIZE 20
#define MAX_ROLE 100
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

void assign_role(char *user,char *role_name){
	int i=0;
	int role=-1;
	for(i=0;i<idx;i++){
		if(strncmp(role_list[i].name,role_name,MAX_CHAR_SIZE)==0){	
		 	role=role_list[i].role;
		}

	}
	if(role==-1){
		printf("The role %s does not exists\n",role_name);
		return;
	}
	write_policy(user,role);

}
int write_role(){
	int ret=0;
	int i=0;
	char *new_line="\n";	
	FILE *ptr=fopen("role.mp","w");
	if(!ptr){
		printf("Could not open the file role.mp to save the role\n");
		printf("The errno is %d\n",errno);
		return -1;
	}
	for(i=0;i<idx;i++){
		struct role_map role_map=role_list[i];
		ret=fwrite(&role_map,sizeof(struct role_map),1,ptr);
		ret=fwrite(new_line,strlen(new_line),1,ptr);
		if(ret<0){
			printf("Error in writing to file role.mp %d\n",errno);
		}
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
		if(errno==2){
			return 0;
		}
		else{
			printf("Could not open the file role.mp to read the role\n");
			return -1;
		}
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
	int i=0;
	int bool=0;
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
	
	for(i=0;i<idx;i++){
		if(strncmp(role_list[i].name,role_map.name,MAX_CHAR_SIZE)==0){
			printf("The role already exists, will over write\n");
			role_list[i]=role_map;
			bool=1; 
		}
		
	}
	
	//Could not find role in the list
	if(bool==0){
		role_list[idx]=role_map;
		idx++;
	}
	
		
}

void print_help(){
	printf("Welcome to user admin\n");
	printf("-h : print help\n");
	printf("-l : list of existing roles\n");
	printf("-a : -a [username] [role name]\n");
	printf("-c : create a role\n");

}

int main(int argc,char *argv[]){	
	int opt;
	int i=0;
	int flag=0;
	read_role();
	while((opt=getopt(argc,argv,"hlca:"))!=-1){
		switch(opt){
			case 'h':
				print_help();
				flag=1;
				break;

			case 'l':
				print_role_list();
				flag=1;
				break;

			case 'c':
				create_role();
				flag=1;
				break;
			case 'a':
				assign_role(optarg,argv[optind]);
				flag=1;
				break;
				
		}
	}
	write_role();	

	if(flag==0){
		print_help();
		exit(1);
	}
		
}





