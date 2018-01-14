all: t1_store_v t0_store_v R_v1 R_v0
 
t1_store_v: t1_store_v.c
	gcc -std=gnu11 -O2 -o $@ $< -lpthread -D_GNU_SOURCE

t0_store_v: t0_store_v.c
	gcc -std=gnu11 -O2 -o $@ $< -lpthread -D_GNU_SOURCE
 
R_v1: R_v1.c
	gcc -std=gnu11 -O2 -o $@ $< -lpthread -D_GNU_SOURCE

R_v0: R_v0.c
	gcc -std=gnu11 -O2 -o $@ $< -lpthread -D_GNU_SOURCE
    	
clean:
	rm -f *.o t1_store_v t0_store_v R_v1 R_v0
