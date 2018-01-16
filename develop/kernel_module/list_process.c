#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/sched/signal.h>

static int __init listprocess_init(void)
{
    pr_info("list process info\n");
    struct task_struct *task;
  

    for_each_process(task)
        pr_info("%s [%d]\n", task->comm, task->pid);
    return 0;
}

static void __exit listprocess_exit(void)
{
    pr_info("list process info exit\n");
}

module_init(listprocess_init);
module_exit(listprocess_exit);
