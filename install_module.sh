#!/bin/sh
lsmod
rmmod sys_rbac
insmod sys_rbac.ko
lsmod
