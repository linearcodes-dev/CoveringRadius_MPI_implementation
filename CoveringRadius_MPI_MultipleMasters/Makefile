CFLAGS =  -m64 -O3 -DNDEBUG -march=native
CC = mpicxx
FILES = main.cpp Covering_functions.h Data.h LinearCombinations.h Polynomials.h
OBJ = CoveringRadiusMPI

all: $(OBJ) 

$(OBJ): $(FILES)
	$(CC) $(CFLAGS)  $(FILES) -o ./$(OBJ)


