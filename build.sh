g++ -c libraries/dinamicList.c
g++ -c libraries/strings.c
g++ -c libraries/nspLib.c
g++ -c libraries/hungarian/hungarian.c

g++ -c main.c
g++ -o main dinamicList.o strings.o nspLib.o hungarian.o main.o