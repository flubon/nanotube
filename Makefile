MAIN = main.cpp
SOURCE = energy.cpp model.cpp molecule.cpp
CPPENERGY = update.cpp adjacent.cpp
CUDAENERGY = update.cu adjacent.cu
LIB = nano.a
LDFLAGS = -L/usr/lib/cuda/lib64 -lcudart

gpu:
	nvcc -D USE_CUDA $(MAIN) $(SOURCE) $(CUDAENERGY) -o main.out --expt-relaxed-constexpr

cpu:
	mv update.cu update.cpp
	mv adjacent.cu adjacent.cpp
	g++ $(MAIN) $(SOURCE) $(CPPENERGY) -o main.out -g2
	mv adjacent.cpp adjacent.cu
	mv update.cpp update.cu

cpu-mpi:
	mv update.cu update.cpp
	mv update.cu update.cpp
	mpic++ -D USE_MPI $(MAIN) $(SOURCE) $(CPPENERGY) -o main.out
	mv adjacent.cpp adjacent.cu
	mv update.cpp update.cu

gpu-mpi:
	nvcc -D USE_CUDA -lib $(SOURCE) $(CUDAENERGY) --expt-relaxed-constexpr -o $(LIB)
	mpic++ -D USE_MPI $(MAIN) $(LIB) $(LDFLAGS) -o main.out

clean:
	rm $(LIB)
	rm *.dump
	rm -rf build
	rm *.out *.a