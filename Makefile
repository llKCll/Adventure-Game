CFLAGS = -Wall

prog: connerke.buildrooms.c
	
	gcc -o prog connerke.buildrooms.c $(CFLAGS)

prog2: connerke.adventure.c 
	
	gcc -o prog2 connerke.adventure.c -lpthread $(CFLAGS)


clean:
	rm prog
	rm prog2
	
	