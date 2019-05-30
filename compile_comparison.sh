nvcc -std=c++11  bitonic_sort.o GPU_calculation.o Tree.o metrics.o input_data.o  comparison.cpp `pkg-config opencv --libs --cflags opencv`  -o comparison
