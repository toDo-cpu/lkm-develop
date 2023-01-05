obj-m += multi_file.o
multi_file-objs := start.o stop.o

all:

		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:

		make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean