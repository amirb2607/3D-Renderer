build:
	gcc -Wall -g -std=c99 ./src/*.c -o main -lSDL2
run:
	./main
clean:
	rm ./main