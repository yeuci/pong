build:
	gcc -Wall -std=c99 -lSDL2 -o pong ./*.c

run:
	./pong

clean:
	rm pong

dev:
	make build && make run
