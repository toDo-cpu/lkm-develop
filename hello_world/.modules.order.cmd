cmd_/home/uwu/Documents/develop/kernel/hello_world/modules.order := {   echo /home/uwu/Documents/develop/kernel/hello_world/hello.ko;   echo /home/uwu/Documents/develop/kernel/hello_world/hello-2.ko; :; } | awk '!x[$$0]++' - > /home/uwu/Documents/develop/kernel/hello_world/modules.order
