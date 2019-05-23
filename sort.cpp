#include <iostream>
#include <time.h>
#include <stdlib.h>
#include "bitonic_sort.h"
#include <algorithm>
#include <limits.h>
#include <opencv2/core.hpp>

#define THREADS 512
#define SMALL_NUM 4000

using namespace std;

void rand_array(float **arrs, int num, int length){
    srand(time(NULL));
    for(int i = 0; i < length; i++){
        float value = (float) rand() / (float) RAND_MAX;
        for(int j = 0; j < num; j++){
            arrs[j][i] = value;
        }
    }
}

void rand_array_with_follower(float **arrs, int *followers, int num, int length){
    srand(time(NULL));
    for(int i = 0; i < length; i++){
        float value = (float) rand() / (float) RAND_MAX;
        for(int j = 0; j < num; j++){
            arrs[j][i] = value;
        }
    }

    for(int i = 0; i < length; i++){
        followers[i] = i;
    }
}

int main(){
    for(int num = 1 << 12; num < 1 << 23; num <<= 1){
        cout<<num<<endl;
        float *values_1 = (float *)malloc(num * sizeof(float));
        float *values_2 = (float *)malloc(num * sizeof(float));
        float *arrs[2] = {values_1, values_2};
        rand_array(arrs, 2, num);

        clock_t start = clock();
        bitonic_sort(values_1, num, num / THREADS, THREADS);
        cout<<"Time: "<<((double) (clock() - start)) / CLOCKS_PER_SEC<<" seconds."<<endl;

        start = clock();
        sort(values_2, values_2 + num);
        int i;
        for(i = 0; i < num; i++){
            if(values_1[i] != values_2[i]){
                cout<<"Wrong! "<<i<<endl;
            }
        }
        if(i == num){
            cout<<"Time: "<<((double) (clock() - start)) / CLOCKS_PER_SEC<<" seconds."<<endl;
        }
    }

    int pow = 1;
    while(pow < SMALL_NUM){pow <<= 1;}
    float *values_1 = (float *)malloc(pow * sizeof(float));
    float *values_2 = (float *)malloc(pow * sizeof(float));
    float *values_3 = (float *)malloc(pow * sizeof(float));
    float *arrs[3] = {values_1, values_2, values_3};
    int *followers = (int *)malloc(pow * sizeof(int));
    rand_array_with_follower(arrs, followers, 2, SMALL_NUM);

/*
    for(int i = 0; i < SMALL_NUM; i++){
        cout<<values_1[i]<<" "<<followers[i]<<endl;
    }
*/
    
    bitonic_sort_with_follower(values_1, followers, SMALL_NUM, pow, 32, pow / 32);

    for(int i = SMALL_NUM; i < pow; i++){
        values_2[i] = INT_MAX;
    }

    cv::Mat tmp_mat(1, pow, CV_32FC1, values_2);
    cout<<tmp_mat.size()<<endl;
    cv::Mat _index_;
    cv::sortIdx(tmp_mat, _index_, CV_SORT_EVERY_ROW + CV_SORT_ASCENDING);
    sort(values_3, values_3 + pow);

    for(int i = 0; i < pow; i++){
        cout<<values_1[i]<<" "<<values_2[i]<<" "<<followers[i]<<" "<<_index_.at<int>(0, i)<<endl;
    }
}
