#include <iostream>
#include "GPU_calculation.h"
#include <time.h>
#include <stdlib.h>

#define LEN 2048
#define NUM 5

using namespace std;

void rand_array(int *arr, int length, int num, int *count){
    srand(time(NULL));
    for(int i = 0; i < length; i++){
        arr[i] = rand() % num;
        count[arr[i]]++;
    }
}

int main(){
    int *arr = (int *)malloc(LEN * sizeof(int));
    int count[NUM] = {0};

    rand_array(arr, LEN, NUM, count);

    for(int i = 0; i < NUM; i++){
        cout<<count[i]<<endl;
    }

    int return_count[NUM] = {0};
    gini_calculation(arr, LEN, NUM, 2, 1024, return_count);

    for(int i = 0; i < NUM; i++){
        cout<<return_count[i]<<endl;
    }
}
