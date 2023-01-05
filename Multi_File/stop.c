#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static void __exit mod_exit(void)
{
	pr_info("mod_exit(void) called.");	
}

module_exit(mod_exit);