#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include<linux/slab.h> //kmalloc kfree

MODULE_AUTHOR("Ahmad AUDI");
MODULE_DESCRIPTION("character readwritebuf driver");
MODULE_SUPPORTED_DEVICE("none");
MODULE_LICENSE();


// cat /proc/devices pour afficher les majeurs des différentes modules
// mknod /dev/aaudidev c 100 0 ---> créer le fichier spécial

//dmesg pour afficher les messages avec printk

static int major = 100;

/* Is the device open right now? Used to prevent 
 * concurent access into the same device */
static int Device_Open = 0;


static int buf_size = 64;
static char *buffer;



//char my_data[80]="hi from kernel"; /* our device */

static ssize_t driver_read(struct file *file, char *buf, size_t count, loff_t *ppos);
static ssize_t driver_write(struct file *file, const char *buf, size_t count, loff_t *ppos);
static int driver_open(struct inode *inode, struct file *file);
//static long driver_ioctl(struct file *f, unsigned int cmd, unsigned long arg);
static int driver_release(struct inode *inode, struct file *file);


static struct file_operations fops = 
{
        .owner =            THIS_MODULE,
        .read = 		    driver_read,
        .write =            driver_write,
        .open = 		    driver_open,
        //.unlocked_ioctl =   driver_ioctl,
        .release =          driver_release,
};


// static ssize_t driver_read(struct file *file, char *buf, size_t count, loff_t *ppos){
//     /* function to copy kernel space buffer to user space*/
// 	if ( copy_to_user(buf, my_data, strlen(my_data)) != 0 )
// 		printk( "Kernel -> userspace copy failed!\n" );
// 	return strlen(my_data);
// }


// static ssize_t driver_write(struct file *file, const char *buf, size_t count, loff_t *ppos){
//     /* function to copy user space buffer to kernel space*/
// 	if ( copy_from_user(my_data, buf, count) != 0 )
// 		printk( "Userspace -> kernel copy failed!\n" );
//     return 0;
// }

static ssize_t driver_read(struct file *file, char *buf, size_t count, loff_t *ppos){

    int lus = 0;

	printk(KERN_DEBUG "read: demande lecture de %zu octets\n", count);
	/* Check for overflow */
	if (count <= buf_size - (int)*ppos)
		lus = count;
	else  lus = buf_size - (int)*ppos;
	if(lus)
		copy_to_user(buf, (int *)buffer + (int)*ppos, lus);
	*ppos += lus;
	printk(KERN_DEBUG "read: %d octets reellement lus\n", lus);
	printk(KERN_DEBUG "read: position=%d\n", (int)*ppos);
    
    int i=0;
    for(i=0; i<buf_size; i++)
		printk(KERN_DEBUG " %c", buffer[i]);
	return lus;

}


static ssize_t driver_write(struct file *file, const char *buf, size_t count, loff_t *ppos){

    int ecrits = 0;
	int i = 0;

	printk(KERN_DEBUG "write: demande ecriture de %zu octets\n", count);
	/* Check for overflow */
	if (count <= buf_size - (int)*ppos)
		ecrits = count;
	else
        ecrits = buf_size - (int)*ppos;

	if(ecrits)
		copy_from_user((int *)buffer + (int)*ppos, buf, ecrits);
	*ppos += ecrits;
	printk(KERN_DEBUG "write: %d octets reellement ecrits\n", ecrits);
	printk(KERN_DEBUG "write: position=%d\n", (int)*ppos);
	printk(KERN_DEBUG "write: contenu du buffer\n");
	for(i=0; i<buf_size; i++)
		printk(KERN_DEBUG " %c", buffer[i]);
	printk(KERN_DEBUG "\n");
	return ecrits;
    
}



static int driver_open(struct inode *inode, struct file *file){
    
    if (Device_Open)
        return -EBUSY;
    Device_Open++;

    printk("nb_access %d\n", Device_Open);
    
    return 0;
}

static int driver_release(struct inode *inode, struct file *file){
    printk(KERN_DEBUG "close()\n");
    return 0;
}

static int __init init_my_module(void){
    
    buffer = kmalloc(buf_size, GFP_KERNEL);
    int ret = register_chrdev(major, "aaudidev", &fops);  
    if(ret < 0){
        printk(KERN_WARNING "Problem with major number\n");
        return ret;
    }  
    printk(KERN_DEBUG "my driver is registred successfully\n");
    return 0;
}

static void __exit exit_my_module(void){
    Device_Open--;
    kfree(buffer);
	unregister_chrdev(major, "driver");
    printk(KERN_DEBUG "my driver is unregistred successfully\n");
}

module_init(init_my_module);
module_exit(exit_my_module);