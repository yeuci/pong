build:
	gcc -Wall -std=c99 -lSDL2 -o pong ./*.c

run:
	./pong

clean:
	rm pong

dev:
	make clean && make build && make run
