# Loadable module goals
obj-m += src/hello.o

# Non-loadable goal
# obj-y += ...

# Compiler flags
EXTRA_CFLAGS := -I$(src)/inc
 
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules
clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
