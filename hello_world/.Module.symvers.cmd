cmd_/home/uwu/Documents/develop/kernel/hello_world/Module.symvers := sed 's/\.ko$$/\.o/' /home/uwu/Documents/develop/kernel/hello_world/modules.order | scripts/mod/modpost  -a  -o /home/uwu/Documents/develop/kernel/hello_world/Module.symvers -e -i Module.symvers  -N -T -