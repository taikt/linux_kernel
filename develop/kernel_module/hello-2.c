#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>

static int __init hellokn_2_init(void)
{
    pr_info("hello world 2\n");
    return 0;
}

static void __exit hellokn_2_exit(void)
{
    pr_info("goodbye, world 2\n");
}

module_init(hellokn_2_init);
module_exit(hellokn_2_exit);



