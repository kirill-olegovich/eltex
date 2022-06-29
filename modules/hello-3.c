#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>

static int a = 1;
module_param(a, int, S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
MODULE_PARM_DESC(myint, "An integer");

static int __init start(void) {
    pr_info("\n\na - %d\n\n", a);

    return 0;
}

static void __exit end(void) { pr_info("\n\nGoodbye\n\n"); }

module_init(start);
module_exit(end)

    MODULE_LICENSE("GPL");
MODULE_AUTHOR("Kirill");
MODULE_DESCRIPTION("123");
