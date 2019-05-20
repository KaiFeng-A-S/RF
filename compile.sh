nvcc -std=c++11 -c Tree.cpp
nvcc -c metrics.cpp
nvcc -std=c++11 -c bitonic_sort.cu
nvcc -std=c++11 -c input_data.cpp
nvcc -std=c++11 Tree.o metrics.o bitonic_sort.o  input_data.o main.cpp `pkg-config opencv --libs --cflags opencv` -o main
