#include <linux/init.h>
#include <linux/module.h>

#include <linux/sched.h>       //wake_up_process()
#include <linux/kthread.h>      //kthread_create()、kthread_run()  

MODULE_LICENSE("Dual BSD/GPL");

bool flag = true;

static struct task_struct *task[3];
static void invcache(void)
{
	uint64_t time = 0;
	__asm__(
		"rdtscp\n\t"
		"shl $32,%%rdx\n\t"
		"or %%rdx,%%rax\n\t"
		"movq %%rax, %%rbx\n\t"
		"movq $10000, %%rsi\n\t"
		".L0:\n\t"
		"wbinvd\n\t"
		"dec %%rsi\n\t"
		"cmpq $0, %%rsi\n\t"
		"jne .L0\n\t"
		"rdtscp\n\t"
		"shl $32,%%rdx\n\t"
		"or %%rdx,%%rax\n\t"
		"subq %%rbx, %%rax\n\t"
		"movq %%rax, %0"
		: "=m" (time)
		:
		: "rax", "rbx", "rcx", "rdx", "rsi"
	);
	printk("time: %d\n", time/10000);
	task[0] = NULL;
}

static void FlushAllCL(void)
{
//	while(true)
	int i = 0;
	for(i = 0; i < 10; i++)
	{
//        if(kthread_should_stop()) break;
               
		smp_call_function_single(3, invcache, NULL, true);
		//smp_call_function_single(0, invcache, NULL, true);
	}

	return;
}

static int disableCache_init(void)
{
	int err;
	const char name[] = "FlushCL";
	
	int i;
	for(i = 0; i < 1; i++)
	{
		task[i] = kthread_create(FlushAllCL, NULL, name); 
		if (IS_ERR(task[i])) {  
			printk("Unable to start kernel thread./n");  
			err = PTR_ERR(task[i]);  
			task[i] = NULL;  
			return -1;  
		}
	//	kthread_bind(task[i], 5); 
		wake_up_process(task[i]);
	}
	
	return 0;
}
static void disableCache_exit(void)
{
	int i;
	for(i = 0; i < 1; i++)
	{
		if(task[i])
		{
		    kthread_stop(task[i]);
		    task[i] = NULL;
		}
	}
	flag = false;		
}

module_init(disableCache_init);
module_exit(disableCache_exit);

