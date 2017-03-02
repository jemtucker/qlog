# Loadable module goals
obj-m += hello.o

# Non-loadable goal
# obj-y += ...
 
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
