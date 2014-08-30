#include <linux/module.h>
#include <linux/moduleloader.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/security.h>

#ifdef CONFIG_SECURITY_FARHAN
static struct proc_dir_entry *proc_entry;
static unsigned long buff_size=0;
static int proc_max_size=100;
char proc_msg[100];

struct policy{
	uid_t uid;
	int role;
};



ssize_t my_read_proc(struct file *filp,char *buf,size_t count,loff_t *off){
	memcpy(buf,proc_msg,buff_size);
	//printk("In the read function %d\n",ret);
	return buff_size;
}
ssize_t my_write_proc(struct file *file,char *buf,size_t count,loff_t *ppos){
	
	struct policy *pol=kmalloc(sizeof(struct policy),GFP_KERNEL);
	int size=sizeof(struct policy);
	if(count>size){
		count=size;
	}
	if(copy_from_user(pol,buf,size)){
		return -EFAULT;
	}
	printk("In the write function %d\n",pol->uid);
	kfree(pol);
	return size;
}

struct file_operations proc_fops={
	.read=my_read_proc,
	.write=my_write_proc,
};

static int my_file_perm(struct file *file,int mask){
	//printk(KERN_INFO "In my perm module \n");
	return 0;
}

struct security_operations rbac_ops={
	.file_permission=my_file_perm,
};


static int __init init_mod(void){
	int ret=0;
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
	remove_proc_entry("my_test",NULL);
	printk("removed module\n");
	
}

module_init(init_mod);
module_exit(exit_mod);
MODULE_LICENSE("GPL");
#endif
