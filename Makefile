build:
	gcc -Wall -std=c99 -lSDL2 -lSDL2_ttf -o pong ./*.c

run:
	./pong

clean:
	rm pong

dev:
	make build && make run
