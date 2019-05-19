g++ -c Tree.cpp
g++ -c metrics.cpp
g++ -std=c++11 -c input_data.cpp
g++ -std=c++11 Tree.o metrics.o input_data.o main.cpp `pkg-config opencv --libs --cflags opencv` -o main
