all: ANN ANN-openmp

ANN: ANN.o
		gcc -o ANN ANN.o -lm
ANN.o: ANN.c
		gcc -c ANN.c -lm

ANN-openmp: ANN-openmp.o
		gcc -o ANN-openmp ANN-openmp.o -lm
ANN-openmp.o: ANN-openmp.c
		gcc -c ANN-openmp.c -lm

run: ANN ANN-openmp
	./ANN 9000
	./ANN-openmp 9000
