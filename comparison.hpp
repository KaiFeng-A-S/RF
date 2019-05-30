#pragma once
#ifndef COMPARISON_H
#define COMPARISON_H

#include <time.h>
#include <stdlib.h>
#include <string>

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

std::string* default_feature_names(int num){
    string *names = new string[num];
    for(int i = 0; i < num ;i++){
        names[i] = "feature_" + to_string(i + 1);
    }

    return names;
}

#endif
