/*
 *	Linux kernel module which hook functions in the syscall_table
 */
#include <linux/init.h>
#include <linux/version.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/syscalls.h>
#include <asm/paravirt.h> // include read_cr0() function
#include <linux/kprobes.h>


/* 
	Select the adapted techniques to get the sct address because
	the way to get the sct address is different between kernel versions
*/


#define DEBUG
#define MOD_NAME "[2-DRAGON-DEBUG]"

MODULE_LICENSE("GPL");
MODULE_AUTHOR("UWU");
MODULE_DESCRIPTION("Hook the syscall table");

/*
 *	Module functions
 */

/* Init the lkm */
static int 	__init 	lkm_init(void);

/* Exit the lkm */
static void __exit lkm_exit(void);

module_init(lkm_init);
module_exit(lkm_exit);

/*
 * Manage write protections
 */

/* Write in cr0 register */
static inline void write_forced_cr0(unsigned long new_val);

/* Disable write protections */
static inline void disable_wp(void);

/* Enable write protections */
static inline void enable_wp(void);

/*
 * Locate syscall table
 */

static unsigned long * __sct_ptr;
static struct kprobe  kp = {
	.symbol_name = "kallsyms_lookup_name",
};

static void * get_sct_address(void);

/*
 *	Functions hook
 */
typedef asmlinkage long (*syscall_read_t)(const struct pt_regs *);
static syscall_read_t old_syscall_read;

asmlinkage long hook_syscall_read(const struct pt_regs *);

/*
 * Functions declarations
 */

/* Module functions */
static int __init lkm_init(void)
{
	if(!(__sct_ptr = get_sct_address()))
	{
		pr_alert("%s - locate sys_call_table failed.\n" , MOD_NAME);
		return -EFAULT;
	}
	
	pr_info("%s in lkm_init(): sys call table 64 is at 0x%p", MOD_NAME,__sct_ptr);

	old_syscall_read = (void*)__sct_ptr[__NR_read];

	disable_wp();

	__sct_ptr[__NR_read] = (unsigned long)hook_syscall_read;

	enable_wp();

	return 0;
}

static void __exit lkm_exit(void)
{
	disable_wp();

	__sct_ptr[__NR_read] = (unsigned long)old_syscall_read;

	enable_wp();
}

/* Manage write protections */
static inline void write_forced_cr0(unsigned long new_val)
{

	unsigned long __force_order;

	asm volatile ("mov %0,%%cr0" : "+r" (new_val), "+m"(__force_order));
}
static inline void disable_wp(void)
{

	/* AND oerations between actual value of cr0 and reversed value of 0x10000 (0x01111) */
	write_forced_cr0(read_cr0() & (~0x10000));
}
static inline void enable_wp(void)
{
	write_forced_cr0(read_cr0() | 0x10000);		
}

/* Locate syscall table */
static void * get_sct_address(void)
{
	int kprobe_res = 0;

	typedef unsigned long (*kallsyms_lookup_name_t) (const char *name);
	kallsyms_lookup_name_t kallsyms_lookup_name;

	/* register the kprobe */
	if(( kprobe_res = register_kprobe(&kp)) != 0)
	{
		pr_alert("%s - register_kprobe() failed with %d.\n", MOD_NAME, kprobe_res);
		return NULL;
	}

	kallsyms_lookup_name = (kallsyms_lookup_name_t)kp.addr;

	/* Work done so unregister */
	unregister_kprobe(&kp);

	return kallsyms_lookup_name("sys_call_table");
}

/* Functions hook */
asmlinkage long hook_syscall_read(const struct pt_regs * regs)
{	
	return old_syscall_read(regs);
}	