HEADERS = ecs.h util.h entities.h
CC = gcc
FLAGS = -g -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wall -Werror -Wpedantic -std=c99

default: rpg

util.o: util.c $(HEADERS)
	$(CC) -c util.c -o util.o $(FLAGS)

ecs.o: ecs.c $(HEADERS)
	$(CC) -c ecs.c -o ecs.o $(FLAGS)

main.o: main.c $(HEADERS)
	$(CC) -c main.c -o main.o $(FLAGS)

rpg: main.o ecs.o util.o
	$(CC) main.o ecs.o util.o -o rpg $(FLAGS)

clean:
	-rm -f ecs.o
	-rm -f main.o
	-rm -f rpg
