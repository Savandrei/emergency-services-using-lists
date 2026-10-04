CC=gcc
CFLAGS=-Wall -Wextra -std=c99

SRCS=$(wildcard *.c)
OBJS=$(SRCS:%.c=%.o)


TARGETS=tema1

build: $(OBJS) $(TARGETS)

run:
		./tema1

tema1: tema1.c
		$(CC) $(CFLAGS) $@.o -o $@ -lm

pack:
		zip -FSr SavlovschiAndrei_311CB_Tema1_SDA.zip README Makefile *.c *.h

clean:
		rm -f $(TARGETS) $(OBJS) 311CB_SavlovschiAndrei_Tema1_SDA.zip

valgrind:
		sudo valgrind --leak-check=full  --show-leak-kinds=all ./tema1
