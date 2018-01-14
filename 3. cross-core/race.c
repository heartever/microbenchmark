// *** mfence
// Not count the "movq $3, (%2)\n\t" and "movq (%2), %%r11\n\t": 66-72 cycles
// baseline, no waiting: 266-286 cycles
// protected thread: 140-154, 272-292 cycles
// shadow thread: 144-156, 272-290
// race: > 440 cycles

// *** NO mfence
// protected thread: 77% < 104, 88.5% < 134
// shadow thread: 60% < 72; 92% < 118

#include <stdint.h>
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <stdbool.h>

char counter[0x100000] = {'\0'};

uint64_t measuredtime[20000000] = {0};
/* 
  covert thread to load the data to the cache; bind to core 0
*/
int flag = 0;
void __attribute__ ((noinline)) *ctFunc(void *arg)
{
	printf("*\n");
	__asm__ __volatile__(
		".Lo12:\n\t"
		
		"movq $4, (%1)\n\t"
		
		//"sfence\n\t"
		
		//"movq (%1), %%rax\n\t"
		
		
		
		"jmp .Lo12\n\t"
		
		:
		: "r"(&flag), "r" (&counter[4])
		: "memory"
	);
	
	return ((void *)0);
}

int hist_3[800] = {0};
int hist_4[800] = {0};

#define MIN(a, b) (a < b ? a : b)

void __attribute__ ((noinline)) *ctFunc2(void *arg)
{
	printf("*\n");
	uint64_t result = 0;
	uint64_t time = 0;
	__asm__ __volatile__(
		"movq %3, %%r9\n\t"
		"movq %4, %%r10\n\t"
		        
		"movq $10000000, %%rsi\n\t"
		//"xor %%rcx, %%rcx\n\t"
		".Lo22:\n\t"        
        "rdtscp\n\t"
        "movq %%rax, %%rbx\n\t"
        
		"movq $3, (%2)\n\t"
		
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		
	
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		
		
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		
		
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		
			"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		
		"movq %%rax, %%rax\n\t"
		"movq %%rax, %%rax\n\t"
		
			
	
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		
	
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		
		
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
	  	
	 /* 	
	  	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
				
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t" // 10

		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"  // 20
		
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"  // 30
		
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"  // 40
		
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"  // 50
		
		
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t" // 60
		
		
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t" // 70
		
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		
	//	"sfence\n\t"  // shadow */
		
	/*	"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		"movq (%2), %%rax\n\t"
		
		
		"movq $3, (%2)\n\t"  // store
		"movq (%2), %%rax\n\t"
		"lfence\n\t"	
		"movq (%2), %%rax\n\t"
		"lfence\n\t"	
		"movq (%2), %%rax\n\t"
		"lfence\n\t"
		"movq (%2), %%rax\n\t"
		"lfence\n\t" // protected 
				*/
		
    	"movq (%2), %%r11\n\t"
 	//	"mfence\n\t"
 		"rdtscp\n\t"
	//	"shl $32,%%rdx\n\t"
   //     "or %%rdx,%%rax\n\t" 
   		"subq %%rbx, %%rax\n\t"
        "movq %%rax, (%%r9)\n\t"
       
        "movq %%r11, (%%r10)\n\t"
        
        
	//	"cmpq $3, %%rax\n\t"
	//	"je .Loo23\n\t"
	//	"inc %%rcx\n\t"
	//	".Loo23:\n\t"
	
		"add $8, %%r9\n\t"
		"add $8, %%r10\n\t"
		
		"dec %%rsi\n\t"
		"cmp $0, %%rsi\n\t"
		"jne .Lo22\n\t"
		
	//	"movq %%rcx, %0\n\t"
		
	//	"rdtscp\n\t"
	//	"shl $32,%%rdx\n\t"
     //   "or %%rdx,%%rax\n\t" 
    //    "subq %%rbx, %%rax\n\t"
    //    "movq %%rax, %1\n\t"
		: "=m" (result), "=m" (time)
		: "r" (&counter[4]), "r" (&measuredtime[0]), "r"(&measuredtime[10000000])
		: "rax", "rbx", "rcx", "rdx", "rsi", "r9", "r10", "r11"
	);
//	printf("result: %d\n", result);
//	printf("time: %d cycle(s)\n\n", time/10000000);
	int i; int max_3 = 0; int min_4 = 10000; uint64_t totaltime = 0; int race = 0; int total = 0;
	for(i = 0; i < 10000000; i++)
	{
		int tmp1 = measuredtime[i]; int tmp2 = measuredtime[10000000+i];
	//	printf("No. %d, time: %d, result: %d\n", i, measuredtime[i], measuredtime[10000000+i]);
		
		//hist_3[MIN(tmp1/2, 799)]++;
		if(tmp2 == 3)
		{
			total++;
			hist_3[MIN(tmp1/2, 799)]++;
			if(tmp1 < 500 && max_3 < tmp1) max_3 = tmp1;
		}
		else if(tmp2 == 4)
		{
			race++; total++;
			hist_4[MIN(tmp1/2, 799)]++;
			if(tmp1 > 200 && min_4 > tmp1) min_4 = tmp1;
		}
		totaltime += tmp1;
	}
	
	FILE *f3 = fopen("3.txt", "w");
	FILE *f4 = fopen("4.txt", "w");
	for(i = 0; i < 300; i++)
	{
		printf("%2d:\t\t %5d,\t\t %5d\n", i*2, hist_3[i], hist_4[i]);
		if(i >= 18) fprintf(f3, "%d:%d\n", i*2-36, hist_3[i]);
		if(i >= 18) fprintf(f4, "%d:%d\n", i*2-36, hist_4[i]);
	}
	printf("Average time: %d. Race: %d\%\n", totaltime/10000000, 100*race/10000000);
	
	fclose(f3);
	fclose(f4);
		//printf("No. %d, time: %d, result: %d\n", i, measuredtime[i], measuredtime[10000000+i]);
}

void __attribute__ ((noinline)) *ctFunc3(void *arg)
{
	printf("*\n");
	__asm__ __volatile__(
		".Loo21:\n\t"
		"cmp $0, (%0)\n\t"
		"je .Loo21\n\t"
		
		".Lo2222:\n\t"
		/*"movq $20, %%rsi\n\t"
		"movq %1, %%rax\n\t"
		".Loosd:\n\t"
		"movq $5, (%%rax)\n\t"
		"add $0x1000, %%rax\n\t"
		"dec %%rsi\n\t"
		"jnz .Loosd\n\t"*/
		"movq $5, (%1)\n\t"
	//	"mfence\n\t"
		
		"jmp .Lo2222\n\t"
		:
		: "r"(&flag), "r" (&counter[4+0x5000])
		: "rax", "memory"
	);
	
	return ((void *)0);
}

pthread_t thread1;
void htthread1(void)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(4, &cpuset);
	pthread_create(&thread1, NULL, &ctFunc, NULL);
	pthread_setaffinity_np(thread1, sizeof(cpu_set_t), &cpuset);
}

pthread_t thread2;
void htthread2(void)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(7, &cpuset);
	pthread_create(&thread2, NULL, &ctFunc2, NULL);
	pthread_setaffinity_np(thread2, sizeof(cpu_set_t), &cpuset);
}

pthread_t thread3;
void htthread3(void)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(2, &cpuset);
	pthread_create(&thread3, NULL, &ctFunc3, NULL);
	pthread_setaffinity_np(thread3, sizeof(cpu_set_t), &cpuset);
}

int main(int argc, char** argv)
{
//  opthread1();
 // opthread2();
  htthread1();
//  htthread3();
  sleep(1);
  
  htthread2();
  
  usleep(1000);
  pthread_join(thread2, NULL );
  printf("counter: %x\n", counter);
  return 0;
}
