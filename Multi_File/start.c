#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RICK ASTLEY");
MODULE_DESCRIPTION("https://www.youtube.com/watch?v=dQw4w9WgXcQ");

static int __init mod_init(void)
{

	pr_debug("mod_init(void) called.");
	return 0;

}

module_init(mod_init);
