HEADERS = ecs.h util.h common_entities.h
CC = gcc
FLAGS = -g -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wall -Wpedantic -std=gnu99 # -Werror

default: rpg

common_entities.o: common_entities.c $(HEADERS)
	$(CC) -c common_entities.c -o common_entities.o $(FLAGS)

util.o: util.c $(HEADERS)
	$(CC) -c util.c -o util.o $(FLAGS)

ecs.o: ecs.c $(HEADERS)
	$(CC) -c ecs.c -o ecs.o $(FLAGS)

main.o: main.c $(HEADERS)
	$(CC) -c main.c -o main.o $(FLAGS)

rpg: main.o ecs.o util.o common_entities.o
	$(CC) main.o ecs.o util.o common_entities.o -o rpg $(FLAGS)

clean:
	-rm -f util.o
	-rm -f ecs.o
	-rm -f main.o
	-rm -f rpg
