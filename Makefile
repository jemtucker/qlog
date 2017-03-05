# Main module target
obj-m += qlog.o

qlog-objs += src/qlog.o src/keylogger.o

# Compiler flags
EXTRA_CFLAGS := -I$(src)/inc
 
all:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) modules

	$(CC) test/test_qlog.c -o test/test_qlog

clean:
	make -C /lib/modules/$(shell uname -r)/build/ M=$(PWD) clean
	rm test/test_qlog
