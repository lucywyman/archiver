CPPC=clang
CFLAGS= -Wall -std=c99 -g

all: myoscar.c
	$(CPPC) $(CFLAGS) myoscar.c

myoscar: myoscar.o
	$(CPPC) $(CFLAGS) myoscar.o

myoscar.o: myoscar.c
	$(CPPC) $(CFLAGS) myoscar.c -c

clean:
	rm -rf ./*.o ./a.out

