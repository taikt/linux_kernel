#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched/signal.h>

MODULE_LICENSE("GPL");

static int __init currentThread_init(void)
{
    pr_info("current thread info\n");
    struct task_struct *task = current;

    pr_info("%s [%d]\n", task->comm, task->pid);
    
    return 0;
}

static void __exit currentThread_exit(void)
{
    pr_info("current thread info exit\n");
}

module_init(currentThread_init);
module_exit(currentThread_exit);
