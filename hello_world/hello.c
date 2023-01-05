#include <linux/kernel.h> /* need for pr_info */
#include <linux/module.h> /* required by all modules */

MODULE_LICENSE("GPL");

int init_module(void)
{
	pr_info("Hello world !\n");
	return 0;
}

void cleanup_module(void)
{
	pr_info("Goodbye world !\n");
}

