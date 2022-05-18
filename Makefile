CC = gcc

bin: main.c
	CC $< -o $@ -lpthread

.PHONY: run clean

run:
	./bin

clean:
	rm bin