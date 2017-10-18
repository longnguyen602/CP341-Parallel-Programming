all: ANN

ANN: ANN.o
		gcc -o ANN ANN.o -lm
ANN.o: ANN.c
		gcc -c ANN.c -lm

run: ANN
	./ANN 9000
