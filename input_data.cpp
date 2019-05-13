#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <string.h> 
#include "input_data.hpp"

using namespace std;

string Trim(string &str){
    str.erase(0, str.find_first_not_of("\t\r\n"));
    str.erase(str.find_last_not_of("\t\r\n") + 1);
    
    return str;
}

int get_data_from_csv(string &line, float **_data_){
    istringstream sin(Trim(line));
    vector<float> datas;
    string field;

    int n = 0;
    while(getline(sin, field, ',')){
        datas.push_back(stof(field, NULL));
        n++;
    }

    float *buffer = new float[datas.size()];
    memcpy(buffer, &datas[0], datas.size() * sizeof(float));
    *_data_ = buffer;
    
    return n;
}

int get_label_from_csv(string &line, int **_label_){
    istringstream sin(Trim(line));
    vector<int> labels;
    string field;

    int n = 0;
    while(getline(sin, field, ',')){
        labels.push_back(stoi(field, NULL));
        n++;
    }

    int *buffer = new int[labels.size()];
    memcpy(buffer, &labels[0], labels.size() * sizeof(int));
    *_label_ = buffer;
    
    return n;
}
