all: store load nop loadlfence

store: store.c
	gcc -std=gnu11 -O2 -o $@ $< -lpthread -D_GNU_SOURCE

load: load.c
	gcc -std=gnu11 -O2 -o $@ $< -lpthread -D_GNU_SOURCE
  
nop: nop.c
	gcc -std=gnu11 -O2 -o $@ $< -lpthread -D_GNU_SOURCE
  
loadlfence: loadlfence.c
	gcc -std=gnu11 -O2 -o $@ $< -lpthread -D_GNU_SOURCE
	
clean:
	rm -f *.o store load nop loadlfence
