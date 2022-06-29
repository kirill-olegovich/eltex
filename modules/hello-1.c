#include <linux/kernel.h>
#include <linux/module.h>

int init_module(void) {
    pr_info("\n\nHello\n\n");

    return 0;
}

void cleanup_module(void) { pr_info("\n\nGoodbye\n\n"); }

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill");
MODULE_DESCRIPTION("123");
