#any questions see: http://opensourceforu.com/2012/06/gnu-make-in-detail-for-beginners/

CC = g++
CFLAGS = -Wall -g
DEPS = libraries/dinamicList.h libraries/strings.h libraries/nspLib.h libraries/hungarian/hungarian.h libraries/cost_matrix.h libraries/operators.h
OBJ = main.o libraries/dinamicList.o libraries/strings.o libraries/nspLib.o libraries/hungarian/hungarian.o libraries/cost_matrix.o libraries/operators.o

main: $(OBJ)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c $(DEPS)
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	@echo "Cleaning up.."
	rm -rf *.o
	rm -rf libraries/*.o
	rm all
	
#go: libraries/dinamicList.c libraries/strings.c libraries/nspLib.c libraries/hungarian/hungarian.c main.c
#	g++ -o main libraries/dinamicList.c libraries/strings.c libraries/nspLib.c libraries/hungarian/hungarian.c main.c
