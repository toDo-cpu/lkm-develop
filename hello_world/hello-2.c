#include <linux/init.h>		/* include macros */
#include <linux/kernel.h>	/* include output functions */
#include <linux/module.h>	/* Required for all modules */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RICK ASTLEY");
MODULE_DESCRIPTION("https://www.youtube.com/watch?v=dQw4w9WgXcQ");

static int __init hello_init(void)
{
	
	pr_info("Hello world 2\n");
	return 0;

}

static void __exit hello_exit(void)
{

	pr_info("Goodbye 2 \n");
}

module_init(hello_init);
module_exit(hello_exit);