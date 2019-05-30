#include <iostream>
#include <string>
#include <opencv2/core.hpp>
#include <stdlib.h>
#include <fstream>
#include "comparison.hpp"
#include "bitonic_sort.h"
#include "GPU_calculation.h"
#include <sstream>
#include "Tree.hpp"
#include "input_data.hpp"
#include "metrics.hpp"

#define REPEAT 100
#define THREADS 1024
#define NUM 2

using namespace std;
using namespace FK; 

void compare_sort(ofstream &fout){
   for(int num = 1 << 12; num < 1 << 23; num <<= 1){
        cout<<num<<endl;
        int pow = 1;
        while(pow < num){pow <<= 1;}
        int threads = THREADS < pow ? THREADS : pow;
        float *values_1 = (float *)malloc(pow * sizeof(float));
        float *values_2 = (float *)malloc(pow * sizeof(float));
        float *arrs[2] = {values_1, values_2};
        int *followers = (int *)malloc(pow * sizeof(int));
        rand_array_with_follower(arrs, followers, 2, num);
        bitonic_sort_with_follower(values_1, followers, num, pow, threads, pow / threads);

        free(values_1);
        free(values_2);
        free(followers);

        double time_summation_1 = 0.0;
        double time_summation_2 = 0.0;
        for(int i = 0; i < REPEAT; i++){
            values_1 = (float *)malloc(pow * sizeof(float));
            values_2 = (float *)malloc(pow * sizeof(float));
            followers = (int *)malloc(pow * sizeof(int));
            rand_array_with_follower(arrs, followers, 2, num);

            clock_t start = clock();
            //bitonic_sort(values_1, num, num / threads, threads);
            bitonic_sort_with_follower(values_1, followers, num, pow, pow / threads, threads);
            time_summation_1 += (double) (clock() - start);

            cv::Mat tmp_mat(num, 1, CV_32FC1, values_2);
            cv::Mat _index_;
            start = clock();
            cv::sortIdx(tmp_mat, _index_, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
            cv::sort(tmp_mat, tmp_mat, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
            time_summation_2 += (double) (clock() - start);

            free(values_1);
            free(values_2);
            free(followers);
        }
            
        double _time_ = (time_summation_1 / REPEAT) / CLOCKS_PER_SEC;
        cout<<"Time: "<<_time_<<" seconds."<<endl;
        fout<<num<<", "<<_time_<<endl;

        _time_ = (time_summation_2 / REPEAT) / CLOCKS_PER_SEC;
        cout<<"Time: "<<_time_<<" seconds."<<endl;
        fout<<num<<", "<<_time_<<endl;
    }

    for(int num = 1e3; num < 1e8; num *= 10){
        cout<<num<<endl;
        int pow = 1;
        while(pow < num){pow <<= 1;}
        cout<<pow<<endl;
        int threads = THREADS < pow ? THREADS : pow;
        float *values_1 = (float *)malloc(pow * sizeof(float));
        float *values_2 = (float *)malloc(pow * sizeof(float));
        float *arrs[2] = {values_1, values_2};
        int *followers = (int *)malloc(pow * sizeof(int));
        rand_array_with_follower(arrs, followers, 2, num);
        bitonic_sort_with_follower(values_1, followers, num, pow, threads, pow / threads);

        free(values_1);
        free(values_2);
        free(followers);

        double time_summation_1 = 0.0;
        double time_summation_2 = 0.0;
        for(int i = 0; i < REPEAT; i++){
            values_1 = (float *)malloc(pow * sizeof(float));
            values_2 = (float *)malloc(pow * sizeof(float));
            followers = (int *)malloc(pow * sizeof(int));
            rand_array_with_follower(arrs, followers, 2, num);

            clock_t start = clock();
            //bitonic_sort(values_1, num, num / threads, threads);
            bitonic_sort_with_follower(values_1, followers, num, pow, pow / threads, threads);
            time_summation_1 += (double) (clock() - start);

            cv::Mat tmp_mat(num, 1, CV_32FC1, values_2);
            cv::Mat _index_;
            start = clock();
            cv::sortIdx(tmp_mat, _index_, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
            cv::sort(tmp_mat, tmp_mat, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
            time_summation_2 += (double) (clock() - start);

            free(values_1);
            free(values_2);
            free(followers);
        }

        double _time_ = (time_summation_1 / REPEAT) / CLOCKS_PER_SEC;
        cout<<"Time: "<<_time_<<" seconds."<<endl;
        fout<<num<<", "<<_time_<<endl;

        _time_ = (time_summation_2 / REPEAT) / CLOCKS_PER_SEC;
        cout<<"Time: "<<_time_<<" seconds."<<endl;
        fout<<num<<", "<<_time_<<endl;
    }
}

void compare_reduction(ofstream &fout){
    for(int num = 1; num < 1e8; num *= 10){
        cout<<num<<endl;
        int *arr  = (int *)malloc(num * sizeof(int));
        int count[NUM] = {0};
        int return_counts[NUM] = {0};
        rand_array(arr, num, NUM, count);

        double time_summation_1 = 0.0;
        double time_summation_2 = 0.0;
        for(int i = 0; i < REPEAT; i++){
            for(int j = 0; j < NUM; j++){
                count[j] = return_counts[j] = 0;
            }

            clock_t start = clock();
            gini_calculation(arr, num, NUM, ((num - 1) / THREADS) + 1, THREADS, return_counts);
            time_summation_1 += (double) (clock() - start);

            start = clock();
            for(int j = 0; j < num; j++){
                count[arr[j]]++;
            }
            time_summation_2 += (double) (clock() - start);
        }

        double _time_ = (time_summation_1 / REPEAT) / CLOCKS_PER_SEC;
        cout<<"Time: "<<_time_<<" seconds."<<endl;
        fout<<num<<", "<<_time_<<endl;

        _time_ = (time_summation_2 / REPEAT) / CLOCKS_PER_SEC;
        cout<<"Time: "<<_time_<<" seconds."<<endl;
        fout<<num<<", "<<_time_<<endl;

        free(arr);

/*
        for(int i = 0; i < NUM; i++){
            cout<<i<<", "<<count[i]<<", "<<return_counts[i]<<endl;
        }
*/

    }
}

void compare_scan(ofstream &fout){
    for(int num = 1; num < 1e8; num *= 10){
        cout<<num<<endl;
        int *arr = (int *)malloc(num * sizeof(int));
        int count[2] = {0};
        rat_array(arr, num);
        count[0] = num / 2;
        count[1] = num - (num / 2);

        double time_summation_1 = 0.0;
        double time_summation_2 = 0.0;
        for(int i = 0; i < REPEAT; i++){
            float *return_ginis = (float *)malloc(num * sizeof(float));

            clock_t start = clock();
            ginis(arr, count, num, NUM, THREADS, return_ginis);
            time_summation_1 += (double) (clock() - start);

            int left_count[2] = {0};
            start = clock();
            for(int j = 0; j < (num - 1) ;j++){
                left_count[arr[j]]++;

                float purity_l = 0.0;
                for(int k = 0; k < 2; k++){
                    float class_prop = left_count[k] / (j + 1);
                    purity_l += class_prop * class_prop;
                }

                float purity_r = 0.0;
                for(int k = 0; k < 2; k++){
                    float class_prop = (float) (count[k] - left_count[k]) / (num - j - 1);
                    purity_r += class_prop * class_prop;
                }

                return_ginis[j] = 1 - (((float) (j + 1) / num) * purity_l + ((float) (num - j - 1) / num) * purity_r);
            }
            return_ginis[num - 1] = 0.0;
            time_summation_2 += (double) (clock() - start);
            
            free(return_ginis);
        }

        double _time_ = (time_summation_1 / REPEAT) / CLOCKS_PER_SEC;
        cout<<"Time: "<<_time_<<" seconds."<<endl;
        fout<<num<<", "<<_time_<<endl;

        _time_ = (time_summation_2 / REPEAT) / CLOCKS_PER_SEC;
        cout<<"Time: "<<_time_<<" seconds."<<endl;
        fout<<num<<", "<<_time_<<endl;

        free(arr);

/*
        for(int i = 0; i < num; i++){
            cout<<return_ginis[i]<<endl;
        }
*/

    }
}

void compare_tree(ifstream &fin_X, ifstream &fin_y, ofstream &fout){
    string line;
    vector<float *> datas;
    int num_1;

    while(getline(fin_X, line)){
        float **data_line = (float **)malloc(sizeof(float *));
        num_1 = get_data_from_csv(line, data_line);
        datas.push_back(*data_line);
    }

    vector<int *> labels;
    int num_2;

    while(getline(fin_y, line)){
        int **label_line = (int **)malloc(sizeof(int *));
        num_2 = get_label_from_csv(line, label_line);
        labels.push_back(*label_line);
    }

    string *feature_names = default_feature_names(num_1);

    float **input_data = (float **)malloc(datas.size() * sizeof(float *));
    memcpy(input_data, &datas[0], datas.size() * sizeof(float *));
    int *input_label = (int *)malloc(labels.size() * sizeof(int));
    for(int i = 0; i < labels.size(); i++){
        input_label[i] = labels[i][0];
    }

    Tree tree;

    tree.load_data(input_data, input_label, datas.size(), 40, 2, feature_names);

    cout<<"Load data finished"<<endl;

    clock_t start = clock();
    tree.Learn_with_GPU(100, 0, true);
    double _time_ = (double) (clock() - start) / CLOCKS_PER_SEC;
    cout<<"Time: "<<_time_<<" seconds."<<endl;
    fout<<", "<<_time_<<endl;

    cout<<"Learn finished"<<endl;

    cv::Mat *predict_data_ = &tree.data;
    cv::Mat *predict_label_ = &tree.label;
    int tmp_height = predict_label_->size().height;
    int y_true[tmp_height];

    for(int i = 0; i < tmp_height; i++){
        y_true[i] = predict_label_->at<int>(i, 0);
    }

    int *predictions = tree.Predict(predict_data_, predict_label_);

    cout<<"Predict finished"<<endl;

    float acc = accuracy_score(y_true, predictions, tmp_height, true);

    cout<<"accuracy is: "<<acc<<endl;

    start = clock();
    tree.Learn(100, 0);
    _time_ = (double) (clock() - start) / CLOCKS_PER_SEC;
    cout<<"Time: "<<_time_<<" seconds."<<endl;
    fout<<", "<<_time_<<endl;

    cout<<"Learn finished"<<endl;

    predictions = tree.Predict(predict_data_, predict_label_);

    cout<<"Predict finished"<<endl;

    acc = accuracy_score(y_true, predictions, tmp_height, true);

    cout<<"accuracy is: "<<acc<<endl;
}

int main(){

/*
    ofstream fout("./COMPARISON/sort.csv");
    compare_sort(fout);
    fout.close();
*/

/*
    ofstream fout("./COMPARISON/reduction.csv");
    compare_reduction(fout);
    fout.close();
*/

/*
    ofstream fout("./COMPARISON/scan.csv");
    compare_scan(fout);
    fout.close();
*/

    ifstream fin_X("/data/fengkai/GPGPU/mb_X.csv");
    ifstream fin_y("/data/fengkai/GPGPU/mb_y.csv");  
    ofstream fout("./COMPARISON/tree.csv");
    compare_tree(fin_X, fin_y, fout);
    fin_X.close();
    fin_y.close();
    fout.close();

    return 0;
}
