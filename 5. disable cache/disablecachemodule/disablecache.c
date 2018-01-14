#include <linux/init.h>
#include <linux/module.h>
#include <linux/smp.h>

MODULE_LICENSE("Dual BSD/GPL");
static void disableCache(void *info)
{
        printk(KERN_ALERT "Disabling L1 and L2 caches.\n");
        __asm__(".intel_syntax noprefix\n\t"
				"push	 rax\n\t"
				"cli\n\t"
                "movq    rax,cr0\n\t"
                "or     rax,(1 << 30)\n\t"
                "movq    cr0,rax\n\t"
                "wbinvd\n\t"
//				"and     rax,~(1 << 29)\n\t"
                "movq    cr0,rax\n\t"
                "wbinvd\n\t"
                ".att_syntax noprefix\n\t"
				"pop	 rax\n\t"
        : : : "rax" );
 //       return 0;
}
static void enableCache(void *info)
{
        printk(KERN_ALERT "Enabling L1 and L2 caches.\n");
        __asm__(".intel_syntax noprefix\n\t"
                "movq    rax,cr0\n\t"
                "and     rax,~(1 << 30)\n\t"
                "movq    cr0,rax\n\t"
                "wbinvd\n\t"
	//			"or     rax,(1 << 29)\n\t"
                "movq    cr0,rax\n\t"
                "wbinvd\n\t"
                ".att_syntax noprefix\n\t"
        : : : "rax" );
}

static void cache_off(void *info)
{
        asm(
		"push %rax\n\t"
		"movq %cr0,%rax\n\t"
                "or $0x40000000,%rax\n\t"  /* Set CD */
                "movq %rax,%cr0\n\t"
		"wbinvd\n\t"
		"pop  %rax\n\t");
}

static void cache_on(void *info)
{
        asm(
		"push %rax\n\t"
		"movq %cr0,%rax\n\t"
                "and $~(1 << 30),%rax\n\t"  /* UnSet CD */
                "movq %rax,%cr0\n\t"
		"wbinvd\n\t"
		"pop  %rax\n\t");
}

int init_module(void)
{

//	smp_call_function_single(3, cache_off, NULL, true);
	smp_call_function_single(4, cache_off, NULL, true);
	smp_call_function_single(7, cache_off, NULL, true);
 
 return 0;
}

void cleanup_module(void)
{
//	smp_call_function_single(3, cache_on, NULL, true);
	smp_call_function_single(4, cache_on, NULL, true);
	smp_call_function_single(7, cache_on, NULL, true);
}
//module_init(disableCache_init);
//module_exit(disableCache_exit);

