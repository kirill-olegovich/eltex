#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>

static int __init start(void) {
	pr_info("\n\nHello\n\n");

	return 0;
}

static void __exit end(void) {
	pr_info("\n\nGoodbye\n\n");
}

module_init(start);
module_exit(end)

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill");
MODULE_DESCRIPTION("123");

