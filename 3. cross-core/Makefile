all: race baseline

baseline: baseline.c
	gcc -std=gnu11 -O2 -o $@ $< -lpthread -D_GNU_SOURCE

race: race.c
	gcc -std=gnu11 -O2 -o $@ $< -lpthread -D_GNU_SOURCE
	
clean:
	rm -f *.o race baseline