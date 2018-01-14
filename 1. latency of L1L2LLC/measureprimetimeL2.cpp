#define _GNU_SOURCE

////================================================================================================///////////////////
#include <stdio.h>
#include <assert.h>
#include <fcntl.h>
#include <linux/kernel-page-flags.h>
#include <stdint.h>
#include <sys/sysinfo.h>
#include <sys/mman.h>
#include <unistd.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <math.h>

using namespace std;

int PROBECOUNT = 10000;
#define PROBEWARMUP 50
int SLOT = 5000;

#define SLICES 4
#define WAYS 16
double fraction_of_physical_memory = 0.2;

uint64_t targetphyaddr;

typedef uint64_t pointer;

#define POINTER_SIZE       (sizeof(void*) * 8)
#define ADDRESS_ALIGNMENT  6
#define MAX_XOR_BITS       7
// ----------------------------------------------

#define ETA_BUFFER 5
#define MAX_HIST_SIZE 1500

int g_pagemap_fd = -1;
size_t mapping_size;
pointer *mapping;

typedef struct primelist *primelist_t;
struct primelist 
{
	primelist_t probe;
	primelist_t prime;
	int data;
};
// ----------------------------------------------
size_t getPhysicalMemorySize() {
    struct sysinfo info;
    sysinfo(&info);
    return (size_t) info.totalram * (size_t) info.mem_unit;
}

// ----------------------------------------------
const char *getCPUModel() {
    static char model[64];
    char *buffer = NULL;
    size_t n, idx;
    FILE *f = fopen("/proc/cpuinfo", "r");
    while (getline(&buffer, &n, f)) {
        idx = 0;
        if (strncmp(buffer, "model name", 10) == 0) {
            while (buffer[idx] != ':')
                idx++;
            idx += 2;
            strcpy(model, &buffer[idx]);
            idx = 0;
            while (model[idx] != '\n')
                idx++;
            model[idx] = 0;
            break;
        }
    }
    fclose(f);
    return model;
}


// ----------------------------------------------
void setupMapping() {
    mapping_size = (size_t)((double)getPhysicalMemorySize() * fraction_of_physical_memory);  //RAW_DATA_SIZE; //

    if (fraction_of_physical_memory < 0.01)
        mapping_size = 2048 * 1024 * 1024u;
	
	//int fd = open("/dev/simple", O_RDWR); 
	//mapping = (pointer *)mmap((void *)RAW_DATA_OFFSET, RAW_DATA_SIZE, PROT_READ | PROT_WRITE, MAP_FILE | MAP_SHARED, fd, 4096);
    mapping = (pointer *)mmap(NULL, mapping_size, PROT_READ | PROT_WRITE,
                   MAP_POPULATE | MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    assert(mapping != (void *) -1);

    printf("%s", "Initialize large memory block...\n");
    for (size_t index = 0; index < mapping_size; index += 0x40) {
        pointer *temporary = (pointer *)((uint8_t *)(mapping) + index);
        temporary[0] = index;
    }
    printf("%s", " done!\n");
}

// ----------------------------------------------
size_t frameNumberFromPagemap(size_t value) {
    return value & ((1ULL << 54) - 1);
}

// ----------------------------------------------
pointer getPhysicalAddr(pointer virtual_addr) {
    pointer value;
    off_t offset = (virtual_addr / 4096) * sizeof(value);
    int got = pread(g_pagemap_fd, &value, sizeof(value), offset);
    
    assert(got == 8);

    // Check the "page present" flag.
    assert(value & (1ULL << 63));

    pointer frame_num = frameNumberFromPagemap(value);
    return (frame_num * 4096) | (virtual_addr & (4095));
}

// ----------------------------------------------
void initPagemap() {
    g_pagemap_fd = open("/proc/self/pagemap", O_RDONLY);
    assert(g_pagemap_fd >= 0);
}

void usage(char *p) {
  fprintf(stderr, "Usage: %s -a address [-m] [-q] [-p pattern] [-o outputfile] [-s slotsize] [-c probecount] [-f offsets] -n [multipliersscans]\n", p);
  exit(1);
}
 
// ----------------------------------------------
uint64_t rdtsc() 
{
    uint64_t a, d;
    asm volatile (
    	"mfence\n\t"
    	"rdtscp" : "=a" (a), "=d" (d) : : "rcx");
    a = (d << 32) | a;
    return a;
}

void maccess(void* p)
{
  asm volatile ("movq (%0), %%rax\n"
    :
    : "c" (p)
    : "rax");
}

void flush(void* p) 
{
    asm volatile ("clflush 0(%0)\n"
      :
      : "c" (p)
      : "rax");
}

unsigned long gettime() {
  volatile unsigned long time;
  asm ("rdtsc": "=a" (time): : "%edx" );
  return time;
}


/*question: the timestamp counter is 64 bits, only considering the lower 32 bits
 neglects the overflow in the lower part.*/
/* Answer: As long as the delay is less than 2^32 we can use simple modular arithmetic on 32 
 * bit integers. This avoids the overhead of looking at the MSB 32 bits */

void delay(uint32_t start, uint32_t cycles) {
    /*spinning  for N cycles*/
    
    if (start == 0)
        start = gettime();
    while (gettime() - start < cycles)
        ;
}

size_t getslicesmapping(uint64_t phys_addr)  
{
	//static const size_t h1[] = { 17, 18, 20, 22, 24, 25, 26, 27, 28, 30, 32 };
	//static const size_t h0[] = { 18, 19, 21, 23, 25, 27, 29, 30, 31, 32};  // likely to be correct, at least compared with the others
	
	static const size_t h0[] = { 17, 18, 20, 22, 24, 25, 26, 27, 28, 30, 32, 33 };
	static const size_t h1[] = { 19, 22, 23, 26, 27, 30, 31 };
	static const size_t h2[] = { 17, 20, 21, 24, 27, 28, 29, 30, 33 };  // hyperthreading is on
	
	//static const size_t h1[] = { 7, 11, 13, 15, 17, 19, 20, 21, 22, 23, 24, 26, 28, 29, 31, 33, 34 };
	//static const size_t h0[] = { 6, 10, 12, 14, 16, 17, 18, 20, 22, 24, 25, 26, 27, 28, 30, 32, 33 };
	

	//static const size_t h0[] = { 17, 19, 20, 21, 22, 23, 24, 26, 28, 29, 31 };
	//static const size_t h1[] = { 18, 19, 21, 23, 25, 27, 29, 30, 31 };
	
	//static const size_t h0[] = { 6, 10, 12, 14, 16, 17, 18, 20, 22, 24, 25, 26, 27, 28, 30, 32 };
	//static const size_t h1[] = { 7, 11, 13, 15, 17, 19, 20, 21, 22, 23, 24, 26, 28, 29, 31, 33, 34 };
	//static const size_t h0[] = { 6, 10, 12, 14, 16, 17, 18, 20, 22, 24, 25, 26, 27, 28, 30, 32, 33 };
	//static const size_t h1[] = { 6, 7, 10, 11, 12, 13, 14, 15, 16, 18, 19, 21, 23, 25, 27, 29, 30, 31, 32, 34 };

	size_t count = sizeof(h0) / sizeof(h0[0]);
	size_t hash = 0;
	for (size_t i = 0; i < count; i++) 
	{
		hash ^= (phys_addr >> h0[i]) & 1;
	}
	
	//return hash;
	count = sizeof(h1) / sizeof(h1[0]);
	size_t hash1 = 0;
	for (size_t i = 0; i < count; i++) 
	{
		hash1 ^= (phys_addr >> h1[i]) & 1;
	}
	//return (hash1 << 1) | hash;
	
	count = sizeof(h2) / sizeof(h2[0]);
	size_t hash2 = 0;
	for (size_t i = 0; i < count; i++) 
	{
		hash2 ^= (phys_addr >> h2[i]) & 1;
	}
	
	return (hash2 << 2) | (hash1 << 1) | hash;
//	return 1;
	
}


int getcachesetL1(uint64_t phys_addr)
{
	//return ( (phys_addr >> 6) & 0x7ff);   // bit 6~16 for a cache set
//	int tmp = (phys_addr >> 6) & 0x7ff;  // bit 6~15 for a L2 cache set
	int tmp = (phys_addr >> 6) & 0x3f;  // bit 6~11 for a L1 cache set
//	printf("%p\t%p\t%p %p\n", phys_addr, phys_addr >> 6, (phys_addr >> 6) & 0x000003ff, tmp);
	return tmp;   // bit 6~15 for a L2 cache set
}

int getcacheset(uint64_t phys_addr)
{
	//return ( (phys_addr >> 6) & 0x7ff);   // bit 6~16 for a cache set
	int tmp = (phys_addr >> 6) & 0x7ff;  // bit 6~15 for a L2 cache set
//	int tmp = (phys_addr >> 6) & 0x3f;  // bit 6~11 for a L1 cache set
//	printf("%p\t%p\t%p %p\n", phys_addr, phys_addr >> 6, (phys_addr >> 6) & 0x000003ff, tmp);
	return tmp;   // bit 6~15 for a L2 cache set
}

void random_shuffle (uint64_t myvector[], int n)
{
	int i;
	for(i = 0; i < 10; i++)
	{
		int j = rand() % n;
		int k = rand() % n;
		uint64_t tmp = myvector[j];
		myvector[j] = myvector[k];
		myvector[k] = tmp;
	}
}

primelist_t initList() 
{
	return NULL;
}

void insert(primelist_t &list, uint64_t addr)
{
	if(list == NULL)
	{
		printf("inserting first element..\n");
		primelist_t cur = (primelist_t)addr;
		cur->probe = NULL;
		cur->prime = NULL;
		cur->data = 0;
		
		list = cur;
		
		printf("inserted first element..\n");
		return;
	}
	
	primelist_t last = list;
	primelist_t cur = (primelist_t)addr;
	
	while(last->probe != NULL)
		 last = last->probe;
    cur->prime = last;
    last->probe = cur;
    cur->probe = NULL;
    cur->data = 0;
    
    list = cur;
    printf("inserted: %p, previous addr: %p\n", addr, cur->prime);
}

void printlist(primelist_t le) {
	primelist_t tmp = le;
	while (tmp) 
	{
		printf("P:%p  %p \n", tmp, getPhysicalAddr((pointer)tmp));
		tmp = tmp->prime;
	}
 
	putchar ('\n');
}

void printprobelist(primelist_t le) {
	primelist_t tmp = le;
	while (tmp) 
	{
		printf("P:%p  %p \n", tmp, getPhysicalAddr((pointer)tmp));
		tmp = tmp->probe;
	}
 
	putchar ('\n');
}

/*traversing the prime list until the NULL pointer*/
void __attribute__ ((noinline)) prime(primelist_t probeLink) {
  asm __volatile__ (
    "L4:			\n"
 //   "  incl 16(%0)               \n"
    "  mov 8(%0), %0		\n"
    "  test %0, %0		\n"
    "  jne L4			\n"
  : : "r" (probeLink) : );
}


/*traversing the prime list until the NULL pointer*/
void __attribute__ ((noinline)) probe(primelist_t probeLink) {
  asm __volatile__ (
    "L5:			\n"
 //   "  incl 16(%0)               \n"
    "  mov (%0), %0		\n"
    "  test %0, %0		\n"
    "  jne L5			\n"
  : : "r" (probeLink) : );
}

/*traversing the prime list until the NULL pointer*/
int __attribute__ ((noinline)) time_probe(primelist_t probeLink) {
//  int time = 0;
  asm __volatile__ (
	"mfence\n\t"

    "L6:			\n"
    "  mov (%0), %0		\n"
    "  test %0, %0		\n"
    "  jne L6			\n"
    "  mfence\n\t"

    : //"=m" (time)
    : "r" (probeLink) 
    : //"rcx"
    );
   
 //  	printf("count: %d\n", time); 
    return 0;
}

/*assuming 200 cycle is memory latency? thus the cache line is evicted 
 from L3 cache? */
/*traverse the probe list until NULL pointer, and return the number of entries that 
 are contained in level 2 cache*/
int __attribute__ ((noinline)) probe_timelist(primelist_t le) {
  volatile int l = 0;
  
  asm __volatile__ (
    "  xorl %%edi, %%edi	\n"
    "L2:			\n"
    "  mfence                   \n"
    "  lfence                   \n"
    "  rdtsc			\n"
    "  movl %%eax, %%esi	\n"
    "  mov (%1),%1		\n"
    "  rdtscp			\n"
    "  subl %%esi, %%eax	\n"
    "  cmpl $100, %%eax		\n"
    "  jle L3			\n"
    "  incl %%edi		\n"
    "L3:			\n"
    "  test %1, %1		\n"
    "  jne L2			\n"
    "  movl %%edi, %0		\n"
      :"=a" (l)
      :"b" (le)
      :"%edx","%esi", "%rdi", "%ecx");
  return l;
}

void doProbes(primelist_t primeList, primelist_t probeList, char *buf, int count, int warmup, int slot) 
{
    
    /*sequence: probe, prime, prime, delay for N cycles*/
    
    for (int i = -warmup; i < count; i++) 
    {
        uint32_t slotstart = gettime();
        /*question: does the number has too be smaller than a char? */
	/* answer: probe_timelist returns the number of ways that were brought in from memory.  On Intel the maximum value is 16 */
        char t = probe_timelist(probeList);
        if (i >= 0)
            buf[i] = t;
        prime(primeList);
        prime(primeList);
        delay(slotstart, SLOT);
    }
}


// ----------------------------------------------
int main()
{
    int ch;
	int mulComponents = 0;
	char *outname = "out";
	FILE *out=stdout;
	long targetphyaddr = 0x7dc3d70c4;
	char *pattern = "111110000011111";
	int quick = 0;
	int offsets[64];
	int noffsets = 64;
	int mulscans = 8;

	if (outname != NULL)
      out = fopen(outname, "w");
      
    printf("CPU: %s\n", getCPUModel());
    printf("Memory percentage: %f\n", fraction_of_physical_memory);

    srand(time(NULL));
    initPagemap();
    setupMapping();

    printf("Mapping has %zu MB\n", mapping_size / 1024 / 1024);
    
    size_t slice[5];
    size_t set[5];
    size_t L1set[5];
    size_t addr[5];
    addr[0] = targetphyaddr; addr[1] = (targetphyaddr & 0xfff) + 0x460;    // 5b8, 5a0
    addr[2] = (targetphyaddr & 0xfff) + 0x756; addr[3] = (targetphyaddr & 0xfff) + 0x76a;
    addr[4] = (targetphyaddr & 0xfff) + 0x77c;
    
    int i;
    for(i = 0; i < 5; i++)
    {
    	slice[i] = getslicesmapping(addr[i]);
		set[i] = getcacheset(addr[i]);
		L1set[i] = getcachesetL1(addr[i]);
	}
	
	primelist_t primeprobelist[5] = {NULL};
	primelist_t primeLists[5];
	primelist_t probeLists[5];
	
	printf("init prime list finished.\n");
	int count = 0; int countL1 = 0;
	
	uint64_t myvector[5][100];
	
	for(int j = 0; j < 1; j++)
	{
		count = 0;
		printf("slice %d: %d, set: %x\n", j, slice[j], set[j]);
		for (size_t i = 0; i < mapping_size; i += 64/sizeof(mapping[0]))  //
		{
			int tmp1 = getcacheset(getPhysicalAddr((uint64_t)(mapping + i)));
			int tmp2 = getcachesetL1(getPhysicalAddr((uint64_t)(mapping + i)));
			//printf("****** %d, set: %p; target: %p\n", i, tmp, set[0]);
			//if( getPhysicalAddr((uint64_t)(mapping + i)) >> 33 ) continue;
			if (//(getslicesmapping(getPhysicalAddr((uint64_t)(mapping + i))) == slice[j]) &&
						 (tmp2 == L1set[j])  )   //   (tmp1 == set[j]) && 
			{
				
				//insert(primeprobelist, (uint64_t)(mapping + i));
				myvector[j][count++] = (uint64_t)(mapping + i + 0x0000/sizeof(mapping[0]));
				
				while(1)
				{
					myvector[j][count++] = (uint64_t)(mapping + i + count*0x1000/sizeof(mapping[0]));
					printf("count: %d\n", count);
					if(count >= 100) goto exit;
				}
			}
			
		}
		
exit:
		if(i >= mapping_size) exit(0);
		random_shuffle (myvector[j], 100);

		for (int it = 0; it < 9; ++it)
		{
			insert(primeprobelist[j], myvector[j][it]); // L2 cache set
		}
	
		primeLists[j] =  primeprobelist[j];
		probeLists[j] =  primeprobelist[j];
	
		while(probeLists[j]->prime != NULL)
			probeLists[j] = probeLists[j]->prime;

		printf("Gen prime list finished.\n");
		
		// warmup
		int i;
		for (i = 0; i < 1000; i++) 
		{
			probe_timelist(probeLists[j]);
			prime(primeLists[j]);
			prime(primeLists[j]);
		}
	}
	printf("Process id: %d\n", getpid());
	printf("size of primelist: %d\n", sizeof(struct primelist));
	sleep(3);
	printf("Starting\n");
	
    printf("priming..\n");


	count = 0;
	int num = 0;

	for(i = 0; i < 10000000; i++)
	
   // while(1)
    {		
    	uint64_t begin = rdtsc(); int j;
    	for(j = 0; j < 10000; j++)
			time_probe(probeLists[0]);
		
		uint64_t end = rdtsc();
		printf("time: %d\n", (end-begin)/10000);
//		prime(primeLists[0]);
//		prime(primeLists[0]);
//		probe(probeLists[0]);
//		probe(probeLists[0]);
    }	
    return 0;
}

