#include <linux/module.h>
#include <linux/moduleloader.h>
#include <linux/kernel.h>
#include <linux/linkage.h>
#include <linux/proc_fs.h>
#include <linux/uaccess.h>
#include <linux/security.h>

#ifdef CONFIG_SECURITY_FARHAN
static struct proc_dir_entry *proc_entry;

ssize_t my_read_proc(struct file *filp,char *buf,size_t count,loff_t *off){
       
	char *msg="hello world\n";
	int len=strlen(msg);
	if(count>len)
		count=len;
	memcpy(buf,msg,len);
	//printk("In the read function %d\n",ret);
	return len;
}
ssize_t my_write_proc(struct file *file,char *buf,size_t count,loff_t *ppos){
	int size=12;
	char msg[size];
	if(copy_from_user(msg,buf,size)){
		return -EFAULT;
	}
	printk("In the write function %s\n",msg);
	return size;
}

struct file_operations proc_fops={
	.read=my_read_proc,
	.write=my_write_proc,
};

static int my_file_perm(struct file *file,int mask){
	printk("In my perm module \n");
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
