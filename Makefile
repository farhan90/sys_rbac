obj-m += sys_rbac.o

all: sys_rbac user_admin

sys_rbac:
	make -Wall -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

user_admin:
	gcc -Werror user_admin.c -o user_admin

clean: 
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean
