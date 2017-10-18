all: ANN

ANN: ANN.o
		gcc -o ANN ANN.o
ANN.o: ANN.c
		gcc -c ANN.c -lm

run: ANN
	./ANN
