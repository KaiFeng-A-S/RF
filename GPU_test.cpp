#include <iostream>
#include "GPU_calculation.h"
#include <time.h>
#include <stdlib.h>

#define LEN 1000
#define NUM 2

using namespace std;

void rand_array(int *arr, int length, int num, int *count){
    srand(time(NULL));
    for(int i = 0; i < length; i++){
        arr[i] = rand() % num;
        count[arr[i]]++;
    }
}

void rat_array(int *arr, int length){
    for(int i = 0; i < length; i++){
        if(i < (length / 2)){
            arr[i] = 0;
        }
        else{
            arr[i] = 1;
        }
    }
}

int main(){
    int *arr = (int *)malloc(LEN * sizeof(int));
    int count[NUM] = {0};

    rand_array(arr, LEN, NUM, count);

    for(int i = 0; i < NUM; i++){
        cout<<count[i]<<endl;
    }

    int return_counts[NUM] = {0};
    gini_calculation(arr, LEN, NUM, 3, 128, return_counts);

    for(int i = 0; i < NUM; i++){
        cout<<return_counts[i]<<endl;
    }

/*
    for(int i = 0; i < LEN; i++){
        arr[i] = 0;
    }
*/

    rat_array(arr, LEN);
    count[0] = LEN / 2;
    count[1] = LEN - (LEN / 2);
    float *return_ginis = (float *)malloc(LEN * sizeof(float));
    ginis(arr, count, LEN, NUM, 128, return_ginis);

    for(int i = 0; i < LEN; i++){
        cout<<return_ginis[i]<<endl;
    }

    return 0;
}
