#include <stdint.h>
#include <stdio.h>
#include <sched.h>
#include <pthread.h>
#include <stdbool.h>

char counter[0x1000000] = {'\0'};

const uint64_t measure_count = 2000000;
//uint32_t measure[2000000] = {0};
uint32_t measure[1];

uint32_t test = 0;
#define MIN(X,Y) (((X) < (Y)) ? (X) : (Y))

/* 
  covert thread to load the data to the cache; bind to core 0
*/
int flag = 0;
void __attribute__ ((noinline)) *ctFunc(void *arg)
{
	printf("*\n");
	uint64_t result = 0;
	uint64_t time = 0;
	__asm__ __volatile__(
		".L1o21:\n\t"
		"cmp $0, (%2)\n\t"
		"je .L1o21\n\t"
		
		"rdtscp\n\t"
		"shl $32,%%rdx\n\t"
		"or %%rdx,%%rax\n\t"
		"movq %%rax, %%rbx\n\t"
		"mfence\n\t"
				
		"movq $10000000, %%rsi\n\t" // number of co-location test
		"xor %%rcx, %%rcx\n\t"
		
	//	"movq %4, %%r9\n\t"            // point to array to record the counter number
	//	"movq $0x10000000, %%rdx\n\t"  
		".L122:\n\t"
		
		
		
		"movq (%3), %%rax\n\t"
			
				
		"movq $0, %%r12\n\t"
		"movq $0, %%r11\n\t"
		
		"cmpq $70, %%rax\n\t"
		"cmovg %%rbx, %%r12\n\t"
		
		"subq %%rax, %%r9\n\t"
		"cmpq $1, %%r9\n\t"
		"cmova %%r11, %%r12\n\t"
		
		"addq %%r12, %%rcx\n\t"
		
		"shlq $8, %%rbx\n\t"   
		"movq %%rax, %%r9\n\t"
		
		
		"movq %%rsi, (%3)\n\t"  // store
		
		
		
		"movq (%3), %%rax\n\t"
		"lfence\n\t"	
		"movq (%3), %%rax\n\t"
		"lfence\n\t"	
		"movq (%3), %%rax\n\t"
		"lfence\n\t"	
		"movq (%3), %%rax\n\t"
		"lfence\n\t"
		"movq (%3), %%rax\n\t"
		"lfence\n\t"
		
		"jmp .L122\n\t"
		"movq %%rcx, %0\n\t"
		
		"rdtscp\n\t"
		"shl $32,%%rdx\n\t"
		"or %%rdx,%%rax\n\t"
		"subq %%rbx, %%rax\n\t"
		"mfence\n\t"
		"movq %%rax, %1\n\t"
		
		: "=m" (result), "=m" (time)
		: "r"(&flag), "r" (&counter[0x4]), "r" (&measure[0])
		: "memory", "rax", "rbx", "rcx", "rdx", "r9"
	);
	printf("******  shadow thread result: %d\n", result);
	printf("shadow thread time: %f\n", (time+0.0)/10000000.0);
	return ((void *)0);
}

void *ctFunc2(void *arg)
{
	printf("*\n");
	uint64_t result = 0;
	uint64_t time = 0;
	__asm__ __volatile__(
		".Lo21:\n\t"
		"cmp $0, (%2)\n\t"
		"je .Lo21\n\t"
		
		//"clflush (%3)\n\t"
		//"mfence\n\t"
		"rdtscp\n\t"
		"shl $32,%%rdx\n\t"
		"or %%rdx,%%rax\n\t"
		"movq %%rax, %%r10\n\t"
		"mfence\n\t"
		
	
		"movq $1000000, %%rsi\n\t"
		"xor %%rcx, %%rcx\n\t"
			
		"movq $0x0, %%rdx\n\t"
		
		".L22:\n\t"
		
		
/*		"movq (%3), %%rax\n\t"
		
		
		"movq %%rsi, (%3)\n\t" // store*/
		
		"movq $0, %%r12\n\t"
		"movq $0, %%r11\n\t"
		
		"cmpq $70, %%rax\n\t"
		"cmovl %%rbx, %%r12\n\t"
		
		"subq %%rax, %%r9\n\t"
		"cmpq $1, %%r9\n\t"
		"cmova %%r11, %%r12\n\t"
		
		"addq %%r12, %%rcx\n\t"
		
		"shlq $8, %%rbx\n\t"   
		"movq %%rax, %%r9\n\t"
	
		
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		
		
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
		"nop\n\t"
				
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t" // 10

		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"  // 20
		
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"  // 30
		
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"  // 40
		
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t" // 50
		
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t" // 60
		
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"  // 70 

		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"  // 80 
		
		
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t" // 90
		
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t"
		"movq (%3), %%rax\n\t" // 100
		
		
		"dec %%rsi\n\t"
		"cmp $0, %%rsi\n\t"
		"jne .L22\n\t"
		
	
		"mfence\n\t"
		
		"rdtscp\n\t"
		"shl $32,%%rdx\n\t"
		"or %%rdx,%%rax\n\t"
		"subq %%r10, %%rax\n\t"
		"mfence\n\t"
		"movq %%rax, %1\n\t"
		
		: "=m" (result), "=m" (time)
		: "r"(&flag), "r" (&counter[0x4]), "r" (&measure[1000000])
		: "memory", "rax", "rbx", "rcx", "rdx", "r9", "r10", "r11", "r12"
	);
	printf("******  protected thread result: %d\n", result);
	printf("protected thread time: %f\n", time/1000000.0);
}

void __attribute__ ((noinline)) *ctFunc3(void *arg)
{
	int i = 0;
	for(i = 0; i < 200; i++)
	{
		counter[0+0x5000+i*0x1000] = counter[1+0x5000+i*0x1000] = counter[2+0x5000+i*0x1000] = counter[3+0x5000+i*0x1000] = counter[4+0x5000+i*0x1000] = counter[5+0x5000+i*0x1000] = 123;
	}
	printf("*\n");
	__asm__ __volatile__(
		".Looo21:\n\t"
		"cmp $0, (%0)\n\t"
		"je .Looo21\n\t"
		
		".Lo2222:\n\t"
		"movq $7, %%rsi\n\t"
		"movq %1, %%rax\n\t"
		".Loosd:\n\t"
		"movq $3, (%%rax)\n\t"
		"add $0x1000, %%rax\n\t"
		"dec %%rsi\n\t"
		"jnz .Loosd\n\t"
		//"movq $5, (%1)\n\t"
	//	"mfence\n\t"
		
		"jmp .Lo2222\n\t"
		:
		: "r"(&flag), "r" (&counter[0x4+0x5000])
		: "rax", "memory"
	);
	
	return ((void *)0);
}

void __attribute__ ((noinline)) *ctFunc4(void *arg)
{
	int i = 0;
	for(i = 0; i < 200; i++)
	{
		counter[0+0x5000+i*0x1000] = counter[1+0x5000+i*0x1000] = counter[2+0x5000+i*0x1000] = counter[3+0x5000+i*0x1000] = counter[4+0x5000+i*0x1000] = counter[5+0x5000+i*0x1000] = 123;
	}
	printf("*\n");
	__asm__ __volatile__(
		".Looo41:\n\t"
		"cmp $0, (%0)\n\t"
		"je .Looo41\n\t"
		
		".Lo4222:\n\t"
		"movq $7, %%rsi\n\t"
		"movq %1, %%rax\n\t"
		".Loo4sd:\n\t"
		"movq $5, (%%rax)\n\t"
		"add $0x1000, %%rax\n\t"
		"dec %%rsi\n\t"
		"jnz .Loo4sd\n\t"
		//"movq $5, (%1)\n\t"
	//	"mfence\n\t"
		
		"jmp .Lo4222\n\t"
		:
		: "r"(&flag), "r" (&counter[0x4+0x5000])
		: "rax", "memory"
	);
	
	return ((void *)0);
}

pthread_t thread1;
void htthread1(void)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(3, &cpuset);
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
	CPU_SET(0, &cpuset);
	pthread_create(&thread3, NULL, &ctFunc3, NULL);
	pthread_setaffinity_np(thread3, sizeof(cpu_set_t), &cpuset);
}

pthread_t thread4;
void htthread4(void)
{
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(3, &cpuset);
	pthread_create(&thread4, NULL, &ctFunc4, NULL);
	pthread_setaffinity_np(thread4, sizeof(cpu_set_t), &cpuset);
}

int main(int argc, char** argv)
{
	printf("address: %p\n", &counter[0x4]);
	htthread1();
//	htthread3();
//	htthread4();
//	sleep(1);

	htthread2();

	sleep(1);
	flag = 1;
	pthread_join(thread2, NULL );
	pthread_join(thread1, NULL );
	

	return 0;
}
