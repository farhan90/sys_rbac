#include <linux/module.h>
#include <linux/moduleloader.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/security.h>
#include <linux/sched.h>
#include <linux/cred.h>
#include <linux/uidgid.h>
#include <linux/fsnotify.h>

#define MY_READ 1
#define MY_RDWR 2
#define MY_CRTDEL 3
 
#ifdef CONFIG_SECURITY_FARHAN
static struct proc_dir_entry *proc_entry;
static unsigned long buff_size=0;
char proc_msg[100];


struct policy{
	uid_t uid;
	int role;
};

struct policy_node{
	struct policy *data;
	struct policy_node *next;  
};

struct policy policy_list[10];
struct policy_node *root;


struct policy_node *create_node(struct policy *pol){
	struct policy_node *node=kmalloc(sizeof(struct policy_node),GFP_KERNEL);
	if(node==NULL){
		printk("Cannot create a node\n");
		return NULL;
	}
	node->data=kmalloc(sizeof(struct policy),GFP_KERNEL);
	if(node->data==NULL){
		printk("Cannot allocate memory for data of the node\n");
		return NULL;
	}
	memcpy(node->data,pol,sizeof(struct policy));
	return node;

}

void insert_node(struct policy_node **root,struct policy_node *node){
	node->next=*root;
	*root=node;

}

struct policy_node *find_node(struct policy_node **root, int uid){
	struct policy_node *ptr=*root;
	while(ptr){
		if(ptr->data->uid==uid){
			return ptr;
		}
		ptr=ptr->next;
	}	
	return NULL;
}


void free_list(struct policy_node **root){
	while(*root){
		struct policy_node *temp=*root;
		*root=temp->next;
		kfree(temp->data);
		kfree(temp);
	}
}

void print_list(struct policy_node **root){
	struct policy_node *ptr=*root;
	while(ptr){
		printk("The user id is %d and the role is %d\n",ptr->data->uid,ptr->data->role);
		ptr=ptr->next;
	}
}

void handle_data(struct policy *pol){
	struct policy_node *ptr=find_node(&root,pol->uid);
	if(ptr==NULL){
		printk("The user does not exist in the list so will be inserted\n");
		
		ptr=create_node(pol);
		insert_node(&root,ptr);
	}
	else{
		printk("The user already exists and will overwrite data\n");
		memcpy(ptr->data,pol,sizeof(struct policy));
	}


}

int user_perm(int uid){
	struct policy_node *ptr=find_node(&root,uid);

	if(ptr==NULL){
		return -1;
	}

	return ptr->data->role;

}

ssize_t my_read_proc(struct file *filp,char *buf,size_t count,loff_t *off){
	memcpy(buf,proc_msg,buff_size);
	//printk("In the read function %d\n",ret);
	return buff_size;
}

ssize_t my_write_proc(struct file *file,char *buf,size_t count,loff_t *ppos){
	struct policy pol;
	int size=sizeof(struct policy);
	if(count>size){
		count=size;
	}
	if(copy_from_user(&pol,buf,size)){
		return -EFAULT;
	}
	printk("In the write function %d\n",pol.uid);
	
	handle_data(&pol);
	print_list(&root);
	return size;
}

struct file_operations proc_fops={
	.read=my_read_proc,
	.write=my_write_proc,
};

static int my_inode_perm(struct inode *inode,int mask){
	
	int uid=current->cred->uid.val;
	int perm=user_perm(uid);

	if(current->cred->uid.val==0){
		return 0;
	}
	else if(perm<0){
		return -EPERM;
	}	

	else if(perm==MY_READ && (mask & MAY_WRITE)){
		return -EPERM;
	}
	else
		return 0;
}



static int my_inode_create(struct inode *inode,struct dentry *dentry,umode_t mode){
	if(current->cred->uid.val==0){
		return 0;
	}
	
	if(current->cred->uid.val==500){
		return -EPERM;
	}
	return 0;
}

static int my_inode_unlink(struct inode *dir,struct dentry *dentry){
	if(current->cred->uid.val==0){
		return 0;
	}
	if(current->cred->uid.val==500){
		return -EPERM;
	}
	return 0;
}

static int my_file_perm(struct file *file, int mask){
	return 0;
}

struct security_operations rbac_ops={
	.inode_permission=my_inode_perm,
	.inode_create=my_inode_create,
	.inode_unlink=my_inode_unlink,
	.file_permission=my_file_perm,
};


static int __init init_mod(void){
	int ret=0;
	root=NULL;
	proc_entry=proc_create("my_test",0,NULL,&proc_fops);

	if(proc_entry ==NULL){
		ret=-ENOMEM;
		printk("Could not create the proc file\n");
	}
	else{
		printk("Module loaded successfully\n");
	}
	ret=register_security(&rbac_ops);
	if(ret){
		printk("Cannot register the security\n");
	}else{
		printk("Farhan security module : registered\n");
	}

	return ret;


}

static void __exit exit_mod(void){
	free_list(&root);
	remove_proc_entry("my_test",NULL);
	printk("removed module\n");
	
}

module_init(init_mod);
module_exit(exit_mod);
MODULE_LICENSE("GPL");
#endif
