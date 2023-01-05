/*
 * Create a little char device in /dev/{DEVICE_NAME} with open,close,write,read operations
*/

#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>

/* Macros declarations */

#define DEBUG
#define SUCCESS 0
#define DEVICE_NAME "chardev"
#define BUFFER_LEN 255




/* Functions prototypes */

//Init and exit functions
static int 	__init chardev_init(void);
static void __exit chardev_exit(void);

module_init(chardev_init);
module_exit(chardev_exit);

//Open & close device file operations
static int device_open		(struct inode* , struct file*);
static int device_release	(struct inode*, struct file*);

//Read & write device file operations
static ssize_t device_write	(struct file*, const char __user*, size_t, loff_t *);
static ssize_t device_read	(struct file*, char __user*, size_t, loff_t *);




/* Global var declarations */

static char device_data_buffer[BUFFER_LEN];

//Used for device creations
static struct class *cls;
static dev_t dev_number;
static struct cdev chardev_cdev;

//Files operations struct
struct file_operations fops = {
	.owner = THIS_MODULE,
	.read = device_read,
	.write = device_write,
	.open = device_open,
	.release = device_release
};

// Is the device already open ?
enum {
	CDEV_NOT_USED		= 0,
	CDEV_EXCLUSIVE_OPEN	= 1,
};

static atomic_t already_open = ATOMIC_INIT(CDEV_NOT_USED);




/* Device methods */

/*
	Functions called when some process try to open the /dev/{DEVICE_NAME} file.
	When the function is called, first control if the file is already open and second it 
	increment the counter var and print the values in kernel log
*/

static int device_open	(struct inode* inode, struct file* file)
{
	static int counter = 0;

	#ifdef DEBUG
		pr_info("%s - debug - device_open(%p, %p) called.\n" , DEVICE_NAME , (void*)inode, (void*)file);
	#endif

	if(atomic_cmpxchg(&already_open, CDEV_NOT_USED, CDEV_EXCLUSIVE_OPEN))	/* Check if the file is already opened */
	{
		pr_info("%s - debug - device_open() - /dev/%s is already opened by another process ... \n" , DEVICE_NAME);
		return -EBUSY;
	}

	
	try_module_get(THIS_MODULE);

	pr_info("%s - file opened the %d times", DEVICE_NAME, counter++);

	return SUCCESS;
}

/*
	The function is called when a process try to close the /dev/{DEVICE_NAME} file.
	The function decrement the bDevice_open to significate that the file can be open
*/

static int device_release	(struct inode * inode, struct file * file)
{
	#ifdef DEBUG
		pr_info("%s - debug - device_release(%p, %p) called.\n" , DEVICE_NAME , (void*)inode, (void*)file);
	#endif

	//Set already_open to CDEV_NOT_USED
	atomic_set(&already_open, CDEV_NOT_USED);
	
	module_put(THIS_MODULE);

	return SUCCESS;
}

/*
	The function write data in given buffer located in the user space in the device_data_buffer
	return number of bytes read

*/
static ssize_t device_write	(struct file * file , const char __user* user_buffer, size_t bytes_to_write , loff_t * offset)
{
	#ifdef DEBUG
		pr_info("%s - debug - device_write(%p, %p, %d, %p) called", DEVICE_NAME, (void*)file, (void*)user_buffer, (int)bytes_to_write, (void*)offset);
	#endif

	char *device_data_ptr = device_data_buffer;
	ssize_t len = min(BUFFER_LEN - *offset, bytes_to_write);

	if (len <= 0)
		return 0;

	/* Read data from user_buffer and write it in device_data_buffer */
	if(copy_from_user(device_data_ptr + *offset, user_buffer, len))
		return -EFAULT;

	*offset += len;
	return len;
}

/*
	The function read data from the device_data_buffer to the user_buffer (located in user space)
	Return the bytes read
*/

static ssize_t device_read	(struct file * file, char __user* user_buffer, size_t bytes_to_read, loff_t * offset)
{
	#ifdef DEBUG
		pr_info("%s - debug - device_read(%p, %p, %d, %p) called", DEVICE_NAME, (void*)file, (void*)user_buffer, (int)bytes_to_read, (void*)offset);
	#endif

	char *device_data_ptr = device_data_buffer;
	ssize_t len = min(BUFFER_LEN - *offset, bytes_to_read);

	if(len <= 0)
		return 0;

	/* Read data from device_data_buffer and write it in the user_buffer */
	if(copy_to_user(user_buffer, device_data_ptr + *offset, len))
		return -EFAULT;
	
	*offset += len;
	return len;
}




/* Init and Exit the module/drivers functions */

/*
	The function allocate a major/minor number for the device, then it register the char device with vfs
	and finally it creating the device file on /dev/.
*/

static int 	__init chardev_init(void)
{
	#ifdef DEBUG
		pr_info("%s - debug - chardev_init() called.\n" , DEVICE_NAME);
	#endif

	int alloc_chrdev_region_result;


	/* Create device number */
	alloc_chrdev_region_result = alloc_chrdev_region(&dev_number, 0, 0, DEVICE_NAME);
	if (alloc_chrdev_region_result != 0)
	{
		pr_alert("%s - alert - alloc_chrdev_region() failed with %d.\n", alloc_chrdev_region_result);
		return alloc_chrdev_region_result;
	}

	pr_info("%s - info - major number assigned is %d", DEVICE_NAME, MAJOR(dev_number));

	/* Register the char device with vfs */
	chardev_cdev.owner = THIS_MODULE;
	cdev_init(&chardev_cdev, &fops);
	cdev_add(&chardev_cdev, dev_number, 1);

	/* Creating the device file */
	cls = class_create(THIS_MODULE, DEVICE_NAME);
	device_create(cls, NULL, dev_number, NULL, DEVICE_NAME);

	pr_info("%s - info - device created on /dev/%s", DEVICE_NAME, DEVICE_NAME);

	return SUCCESS;
}

/*
	The function destroy the device file on /dev/, then it unregister the char device with vfs
	and finally it free the major/minor number used by the device.
*/

static void __exit chardev_exit(void)
{
	#ifdef DEBUG
		pr_info("%s - debug - chardev_exit() called.\n" , DEVICE_NAME);
	#endif

	/* Deleting the device file */
	device_destroy(cls, dev_number);
	class_destroy(cls);
	
	/* unregister the char device with vfs */
	cdev_del(&chardev_cdev);

	/* Free device number */
	unregister_chrdev_region(dev_number, 0);
}

/* Meta informations */

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RICK ASTLEY");
MODULE_DESCRIPTION("https://www.youtube.com/watch?v=dQw4w9WgXcQ");