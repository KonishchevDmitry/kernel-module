#define pr_fmt(fmt) "%s: " fmt, KBUILD_MODNAME
//#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/module.h>
#include <linux/sched.h>
#include <linux/sched/signal.h>
#include <linux/preempt.h>
#include <linux/cred.h>
#include <linux/uidgid.h>

MODULE_AUTHOR("Dmitry Konishchev");
MODULE_DESCRIPTION("My Linux development playground");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

static void print_process_info(struct task_struct* process, bool print_threads) {
	{
		task_lock(process);

		uid_t uid = from_kuid(&init_user_ns, process->cred->uid);

		const char* command_prefix = "";
		const char* command_suffix = "";
		if (!process->mm) {
			command_prefix = "[";
			command_suffix = "]";
		}

		pr_info(
			"pid:%5d uid:%4d: %s%s%s\n",
			process->tgid, uid, command_prefix, process->comm, command_suffix
		);

		task_unlock(process);
	}

	if (!print_threads) {
		return;
	}

	struct task_struct* thread;
	for_each_thread(process, thread) {
		if (thread == process) {
			continue;
		}

		task_lock(thread);

		if (strcmp(process->comm, thread->comm)) {
			pr_info("* tid:%5d: %s\n", thread->pid, thread->comm);
		} else {
			pr_info("* tid:%5d\n", thread->pid);
		}

		task_unlock(thread);
	}
}

static void print_all_processes(void) {
	pr_info("All system processes:\n");

	rcu_read_lock();

	// CPU0 idle thread, i.e., the pid 0 task, the (terribly named) swapper/$cpu
	print_process_info(&init_task, true);

	struct task_struct* process;
	for_each_process(process) {
		print_process_info(process, true);
	}

	rcu_read_unlock();
}

static int __init my_init(void) {
	pr_info("The module is inserted by:\n");
	if (in_task()) {
		print_process_info(current, false);
	}

	print_all_processes();
	return 0;
}

static void __exit my_exit(void) {
	pr_info("The module is removed by:\n");
	if (in_task()) {
		print_process_info(current, false);
	}
}

module_init(my_init);
module_exit(my_exit);