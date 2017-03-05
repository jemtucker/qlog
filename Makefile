# Loadable module goals
obj-m += src/hello.o

# Non-loadable goal
# obj-y += ...

# Compiler flags
EXTRA_CFLAGS := -I$(src)/inc
 
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

	$(CC) test/hello.c -o test/test_hello

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm test/test_hello
