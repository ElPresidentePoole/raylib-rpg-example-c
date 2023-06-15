HEADERS = ecs.h ecs_components.h ecs_systems.h util.h common_entities.h
CC = gcc
ifeq ($(TARGET), windows)
	FLAGS = -g -lraylib -lopengl32 -lgdi32 -lwinmm -Wpedantic -std=c99 -Werror
else
	FLAGS = -g -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -Wall -Wpedantic -std=c99 -Werror
endif

default: rpg

common_entities.o: common_entities.c $(HEADERS)
	$(CC) -c common_entities.c -o common_entities.o $(FLAGS)

util.o: util.c $(HEADERS)
	$(CC) -c util.c -o util.o $(FLAGS)

ecs_systems.o: ecs_systems.c $(HEADERS)
	$(CC) -c ecs_systems.c -o ecs_systems.o $(FLAGS)

ecs.o: ecs.c $(HEADERS)
	$(CC) -c ecs.c -o ecs.o $(FLAGS)

main.o: main.c $(HEADERS)
	$(CC) -c main.c -o main.o $(FLAGS)

rpg: main.o ecs.o ecs_systems.o util.o common_entities.o
	$(CC) main.o ecs.o ecs_systems.o util.o common_entities.o -o rpg $(FLAGS)

run:
	./rpg

clean:
	-rm -f common_entities.o
	-rm -f util.o
	-rm -f ecs_systems.o
	-rm -f ecs.o
	-rm -f main.o
	-rm -f rpg
