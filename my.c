#define pr_fmt(fmt) "%s:%s(): " fmt, KBUILD_MODNAME, __func__

#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/sched.h>	/* current() */
#include <linux/preempt.h>	/* in_task() */
#include <linux/cred.h>		/* current_{e}{u,g}id() */
#include <linux/uidgid.h>	/* {from,make}_kuid() */

MODULE_AUTHOR("Dmitry Konishchev");
MODULE_DESCRIPTION("My Linux development playground");
MODULE_LICENSE("GPL");
MODULE_VERSION("0.1");

static void show_ctx(void) {
	/* Extract the task UID and EUID using helper methods provided */
	unsigned int uid = from_kuid(&init_user_ns, current_uid());
	unsigned int euid = from_kuid(&init_user_ns, current_euid());

	if (!in_task()) {
		pr_alert("in interrupt context [Should NOT Happen here!]\n");
		return;
	}

	pr_info("in process context ::\n"
		" PID         : %6d\n"
		" TGID        : %6d\n"
		" UID         : %6u\n"
		" EUID        : %6u (%s root)\n"
		" name        : %s\n"
		" current (ptr to our process context's task_struct) :\n"
		"               0x%pK (0x%px)\n"
		" stack start : 0x%pK (0x%px)\n",
		/* always better to use the helper methods provided */
		task_pid_nr(current), task_tgid_nr(current),
		/* ... rather than the 'usual' direct lookups:
			* current->pid, current->tgid,
			*/
		uid, euid,
		(euid == 0 ? "have" : "don't have"),
		current->comm,
		current, current,
		current->stack, current->stack);
}

static int __init my_init(void)
{
	pr_info("The module is inserted.\n");
	pr_info("sizeof(struct task_struct)=%zd\n", sizeof(struct task_struct));
	show_ctx();
	return 0;
}

static void __exit my_exit(void)
{
	show_ctx();
	pr_info("The module is removed.\n");
}

module_init(my_init);
module_exit(my_exit);