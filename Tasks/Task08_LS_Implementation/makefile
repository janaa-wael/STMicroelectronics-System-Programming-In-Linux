my_ls: LS.o main.o
	gcc -g -o my_ls main.o LS.o 

LS.o: LS.c LS.h
	gcc -c LS.o LS.c

main.o: main.c LS.h LS.c
	gcc -c main.o main.c LS.c

clean:
	rm -f main.o LS.o my_ls

.PHONY: all clean
