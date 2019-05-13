#include <iostream>
#include <string>
#include "Tree.hpp"
#include <opencv2/core.hpp>
#include <stdlib.h>
#include <fstream>
#include <sstream>
#include "input_data.hpp"

using namespace std;
using namespace FK;

int main(){

/*
    cout<<"FK"<<endl;
*/

    Tree tree;

/*
    tree.get_class_number();
    cout<<tree.root->_id_<<endl;
*/

/*
    cv::Mat_<float> A = (cv::Mat_<float>(3,2) << 1, 7, 
                                            2, 8,
                                            3, 9);
    cv::Mat_<float> B = (cv::Mat_<float>(3, 2) << 4, 10, 
                                            5, 11,
                                            6, 12);
    cv::vconcat(A, B, B);
    cout<<B<<endl;

    A.at<float>(0, 0) = 50;
    cout<<A<<endl;
    cout<<B<<endl;
*/

/*
    Tree::TreeNode *tree_node = (Tree::TreeNode *)malloc(sizeof(Tree::TreeNode));
    tree_node->_id_ = 0;
    cout<<tree_node->_id_<<endl;
*/

/*
    cv::Mat col = A.col(0);
    cv::Mat index;
    cout<<col<<endl;
    //cv::sort(col, col, CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);
    cv::sortIdx(col, index, CV_SORT_EVERY_COLUMN + CV_SORT_DESCENDING);
    cout<<col<<endl;
    cout<<index<<endl;
    cout<<index.size().height<<endl;
    cout<<index.type()<<endl;
    cout<<A<<endl;
*/

    ifstream fin_X("/data/fengkai/GPGPU/s_X.csv");
    string line;
    vector<float *> datas;
    int num_1;
    while(getline(fin_X, line)){
        float **data_line = (float **)malloc(sizeof(float *));
        num_1 = get_data_from_csv(line, data_line);
        datas.push_back(*data_line);
    }

/*
    for(int i = 0; i < datas.size(); i++){
        for(int j = 0; j < num_1; j++){
            cout<<datas[i][j]<<endl;
        }
    }
*/

    ifstream fin_y("/data/fengkai/GPGPU/s_y.csv");
    vector<int *> labels;
    int num_2;
    while(getline(fin_y, line)){
        int **label_line = (int **)malloc(sizeof(int *));
        num_2 = get_label_from_csv(line, label_line);
        labels.push_back(*label_line);
    }

/*
    for(int i = 0; i < datas.size(); i++){
        for(int j = 0; j < num_2; j++){
            cout<<labels[i][j]<<endl;
        }
    }
*/

    string feature_names[2] = {"feature_1, feature_2"};
    float **input_data = (float **)malloc(datas.size() * sizeof(float *));
    memcpy(input_data, &datas[0], datas.size() * sizeof(float *));
    int *input_label = (int *)malloc(labels.size() * sizeof(int));
    for(int i = 0; i < labels.size(); i++){
        input_label[i] = labels[i][0];
    }

/*
    for(int i = 0; i < datas.size(); i++){
        stringstream ss;
        for(int j = 0; j < num_1 - 1; j++){
            ss<<input_data[i][j]<<",";
        }
        ss<<input_data[i][num_1 - 1];
        cout<<ss.str()<<endl;
    }
*/

    tree.load_data(input_data, input_label, datas.size(), 2, 2, feature_names);

/*
    cout<<tree.data<<endl;
    cout<<tree.label<<endl;
*/

    tree.Learn(2, 1e-4);

    return 0;
}
