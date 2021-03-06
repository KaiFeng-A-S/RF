#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <float.h>
#include <opencv2/core.hpp>
#include "Tree.hpp"
#include "debug.hpp"
#include "metrics.hpp"
#include "bitonic_sort.h"
#include <climits>
#include "GPU_calculation.h"

#define USE_CPU(A, B) ((A < THRESHOLD_LENGTH) && (B))

using namespace std;

void FK::Tree::split_data(int index, float threshold, cv::Mat *original, cv::Mat *original_label, cv::Mat **first, cv::Mat **second, cv::Mat **first_label, cv::Mat **second_label){
    int tmp_height = original->size().height;
    int tmp_width = original->size().width;

/*
    int count = 0;
    for(int i = 0; i < tmp_height; i++){
        if(original->at<float>(i, index) <= threshold){
            count++;
        }
    }
*/

    cv::Mat *_first_ = new cv::Mat(0, tmp_width, CV_32FC1);
    cv::Mat *_second_ = new cv::Mat(0, tmp_width, CV_32FC1);
    cv::Mat *_first_label_ = new cv::Mat(0, 1, CV_32FC1);
    cv::Mat *_second_label_ = new cv::Mat(0, 1, CV_32FC1);

    for(int i = 0; i < tmp_height; i++){
        if(original->at<float>(i, index) <= threshold){
            _first_->push_back(original->row(i));
            _first_label_->push_back(original_label->row(i));
        }
        else{
            _second_->push_back(original->row(i));
            _second_label_->push_back(original_label->row(i));
        }
    }

/*
    for(int i = 0; i < tmp_height; i++){
        cv::Mat row = original->row(i);
        cv::Mat label_row = original_label->row(i);
        float value = row.at<float>(0, index);

        _dd_("split data", "1");
        cout<<row<<endl;
        cout<<_first_<<endl;
        cout<<_first_.dims<<endl;
        cout<<"value: "<<value<<endl;

        if(value < threshold){
            cv::vconcat(_first_, row, _first_);
            cv::vconcat(_first_label_, label_row, _first_label_);
        }
        else{
            cv::vconcat(_second_, row, _second_);
            cv::vconcat(_second_label_, label_row, _second_label_);
        }
    }

    *first = &_first_;
    *second = &_second_;
    *first_label = &_first_label_;
    *second_label = &_second_label_;
*/

    *first = _first_;
    *second = _second_;
    *first_label = _first_label_;
    *second_label = _second_label_;

/*
    cout<<(*first)->size()<<endl;
    cout<<(*second)->size()<<endl;
*/

} 

float FK::Tree::Gini(cv::Mat *_data_, cv::Mat *_label_, int *return_counts){
    int counts[class_number] = {0};
    float purity = 0.0;
    int tmp_height = _data_->size().height;

/*
    for(int i = 0; i < class_number; i++){
        class_labels[i] = i;
    }
*/

    for(int i = 0; i < tmp_height; i++){
        _label_->at<int>(i, 0);
        counts[_label_->at<int>(i, 0)]++;
    }

    for(int i = 0; i < class_number; i++){
        float class_prop = (float) counts[i] / tmp_height;
        purity += class_prop * class_prop;
        return_counts[i] = counts[i];
    }

    return 1 - purity;
}

float FK::Tree::get_minimum_gini(cv::Mat *_data_, cv::Mat *_label_, int index, float *return_threshold, float *gain){
    cv::Mat tmp_mat = _data_->col(index).clone();
    cv::Mat _index_;
    int tmp_height = tmp_mat.size().height;
    vector<float> ginis;

/*
    cout<<"tmp_height: "<<tmp_height<<endl;
*/

    cv::sortIdx(tmp_mat, _index_, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);
    cv::sort(tmp_mat, tmp_mat, CV_SORT_EVERY_COLUMN + CV_SORT_ASCENDING);

/*
    cout<<tmp_mat<<endl;
*/

    cv::Mat re_label(_label_->size(), _label_->type());
    reorder(_label_, &re_label, &_index_, 0);
    int counts[class_number] = {0};
    float original_gini = Gini(&tmp_mat, &re_label, counts);

/*
    cout<<re_label<<endl;
*/

/*
    for(int i = 0; i < class_number; i++){
        cout<<counts[i]<<endl;
    }
*/

    int tmp_counts[class_number];
    memcpy(tmp_counts, counts, class_number * sizeof(int));

/*
    for(int i = 0; i < class_number; i++){
        cout<<tmp_counts[i]<<endl;
    }
*/

    for(int i = 0; i < (tmp_height - 1); i++){

        /*
        cv::Mat **first = (cv::Mat **)malloc(sizeof(cv::Mat *));
        cv::Mat **second = (cv::Mat **)malloc(sizeof(cv::Mat *));
        cv::Mat **first_label = (cv::Mat **)malloc(sizeof(cv::Mat *));
        cv::Mat **second_label = (cv::Mat **)malooc(sizeof(cv::Mat *));
        float threshold = tmp_mat.at<float>(i, 0);
        Tree::split_data(index, threshold, _data_, _label_, first, second, first_label, second_label);
        float first_gini = Tree::Gini(*first, *first_label, index);
        float second_gini = Tree::Gini(*second, *second_label, index);
        int first_height = (*first)->size().height;
        int second_height = (*second)->size().height;
        float _gini_ = (((float) first_height / tmp_height) * first_gini) + (((float) second_height / tmp_height) * second_gini);
        ginis.push_back(_gini_);
        */

        tmp_counts[re_label.at<int>(i, 0)]--;

/*
        for(int k = 0; k < class_number; k++){
            cout<<tmp_counts[k]<<endl;
        }
*/

        float purity_l = 0.0;
        float purity_r = 0.0;
        for(int j = 0; j < class_number; j++){
            float class_prop_l = (float) (counts[j] - tmp_counts[j]) / (i + 1);
            float class_prop_r = (float) tmp_counts[j] / (tmp_height - i - 1);
            purity_l += class_prop_l * class_prop_l;
            purity_r += class_prop_r * class_prop_r;
        }

        float purity_sum = (((float) (i + 1) / tmp_height) * purity_l) + (((float) (tmp_height - i - 1) / tmp_height) * purity_r);
        ginis.push_back((1 - purity_sum));
    }

    ginis.push_back(original_gini);

/*
    for(int i = 0; i < ginis.size(); i++){
        cout<<ginis[i]<<endl;
    }
*/

/*
    float minimum = ginis[0];
    int minimum_index = 0;
    for(int i = 0; i < tmp_height; i++){
        if(ginis[i] < minimum){
            minimum = ginis[i];
            minimum_index = i;
        }
    }
*/

    int minimum_index = min_element(ginis.begin(), ginis.end()) - ginis.begin();
    *return_threshold = tmp_mat.at<float>(minimum_index, 0);
    float minimum = ginis[minimum_index];
    *gain = original_gini - minimum;

/*
    cout<<"minimum index: "<<minimum_index<<endl;
    cout<<"minimum: "<<minimum<<endl;
    cout<<"threshold: "<<*return_threshold<<endl;
*/

    return minimum;
}

void FK::Tree::get_class_number(){
    cout<<class_number<<endl;
}

FK::Tree::TreeNode* FK::Tree::BuildTree(int depth, cv::Mat *_data_, cv::Mat *_label_){
    if(depth > MAX_DEPTH){
        return NULL;
    }

    FK::Tree::TreeNode *root = (FK::Tree::TreeNode *)malloc(sizeof(FK::Tree::TreeNode));

/*
    cout<<root<<endl;
*/

    //for each feature
    int feature_number = features.size();

/*
    for(int i = 0; i < features.size(); i++){
        cout<<features[i]<<endl;
    }
*/

    int feature_index = 0;
    float minimum = 1.0;
    float return_threshold;
    float return_thres_tmp;
    float _gain_;
    float gain_tmp;

    for(int i = 0; i < feature_number; i++){
        float mini_tmp = get_minimum_gini(_data_, _label_, i, &return_thres_tmp, &gain_tmp);
        if(mini_tmp < minimum){
            minimum = mini_tmp;
            feature_index = i;
            return_threshold = return_thres_tmp;
            _gain_ = gain_tmp;
        }
    }

/*
    cout<<"minimum: "<<minimum<<endl;
    cout<<"feature index: "<<feature_index<<endl;
    cout<<"threshold: "<<return_threshold<<endl;
    cout<<"gain: "<<_gain_<<endl;
*/

/*
    cout<<*_label_<<endl;
    cout<<_data_->col(feature_index)<<endl;
*/

    root->Attribute = &features[feature_index];
    root->index = feature_index;
    root->is_leaf = false;
    root->gain = _gain_;
    root->threshold = return_threshold;

    if(_gain_ <= EPSILON){

/*
        root->is_leaf = true;
        int counts[class_number] = {0};
        int tmp_height = _label_->size().height;
        for(int i = 0; i < tmp_height; i++){
            _label_->at<int>(i, 0);
            counts[_label_->at<int>(i, 0)]++;
        }
        int maximum_index = max_element(counts, counts + class_number) - counts;
        root->classification = maximum_index;
*/

        add_leaf(root, _label_);
                
        return root;
    }

    cv::Mat *first;
    cv::Mat *second;
    cv::Mat *first_label;
    cv::Mat *second_label;

    split_data(feature_index, return_threshold, _data_, _label_, &first, &second, &first_label, &second_label);

    if(first->size().height == 0 || second->size().height == 0){
        add_leaf(root, _label_);

        return root;
    }

/*
    cout<<first->size()<<endl;
    cout<<first_label->size()<<endl;
    cout<<second->size()<<endl;
    cout<<second_label->size()<<endl; 
*/

    root->lch_index = BuildTree(depth + 1, first, first_label);
    root->rch_index = BuildTree(depth + 1, second, second_label);

    if((root->lch_index == NULL) && (root->rch_index == NULL)){
        add_leaf(root, _label_);
    }

/*
    if(root->is_leaf){
        int counts[class_number] = {0};
        int tmp_height = _label_->size().height;
        for(int i = 0; i < tmp_height; i++){
            _label_->at<int>(i, 0);
            counts[_label_->at<int>(i, 0)]++;
        }
        int maximum_index = max_element(counts, counts + class_number) - counts;
        root->classification = maximum_index;
    }
*/

/*
    cout<<"depth: "<<depth<<endl;
*/

    return root;
}

void FK::Tree::reorder(cv::Mat *_original_, cv::Mat *_new_, cv::Mat *_index_, int index){
    int tmp_height = _index_->size().height;
    for(int i = 0; i < tmp_height; i++){

/*
        cout<<_original_->at<int>(i, 0)<<endl;
        cout<<_index_->at<int>(i, 0)<<endl;
        cout<<_original_->at<int>(_index_->at<int>(i, 0), index)<<endl;
        cout<<""<<endl;
*/
        _new_->at<int>(i, 0) = _original_->at<int>(_index_->at<int>(i, 0), 0);
    }
}

void FK::Tree::Learn(int max_depth, float epsilon){
    MAX_DEPTH = max_depth;
    EPSILON = epsilon;
    root = BuildTree(1, &data, &label);
}

int FK::Tree::forward(cv::Mat &_row_){
    FK::Tree::TreeNode *node = root;
    FK::Tree::TreeNode *tmp_node;
    while(!node->is_leaf){
        int index = node->index;
        float value = _row_.at<float>(0, index);
        tmp_node = (value <= node->threshold) ? node->lch_index : node->rch_index;
        if(tmp_node == NULL){break;}
        else{node = tmp_node;}
    }

    return node->classification;
}

int* FK::Tree::Predict(float **_data_, int *_label_, int length){
    return NULL;
}

int* FK::Tree::Predict(cv::Mat *_data_, cv::Mat *_label_){
    int tmp_height = _label_->size().height;
    int *Prediction = (int *)malloc(tmp_height * sizeof(int));
    for(int i = 0; i < tmp_height; i++){
        cv::Mat _row_ = _data_->row(i);
        Prediction[i] = forward(_row_);
    }

    return Prediction;
}

template <class T>
T* FK::Tree::col_to_array(cv::Mat *_data_, int index){
    int tmp_height = _data_->size().height;
    int pow = 1;
    while(pow < tmp_height){pow <<= 1;}

    T* _array_  = (T *)malloc(pow * sizeof(T *));

    for(int i = 0; i < tmp_height; i++){
        _array_[i] = _data_->at<T>(i, index);
    }

    return _array_;
}

void FK::Tree::Gini_with_GPU(int *_label_, int length, int BLOCKS, int THREADS, int *return_counts){
    gini_calculation(_label_, length, class_number, BLOCKS, THREADS, return_counts);

/*
    float purity = 0.0;
    for(int i = 0; i < class_number; i++){
        float class_prop = (float) return_counts[i] / length;
        purity += class_prop * class_prop;
    }

    return 1 - purity;
*/

}

float* FK::Tree::Ginis_with_GPU(int *_label_, int length, int THREADS, int *count){
    float *_gini_ = (float *)malloc(length * sizeof(float));
    ginis(_label_, count, length, class_number, THREADS, _gini_);

    return _gini_;
}

float FK::Tree::get_minimum_gini_with_GPU(cv::Mat *_data_, cv::Mat *_label_, int index, float *return_threshold, float *gain){
    int tmp_height = _label_->size().height;
    float *tmp_data = col_to_array<float>(_data_, index);
    int *tmp_label = col_to_array<int>(_label_, 0);

/*
    cout<<"tmp_height:"<<tmp_height<<endl;
*/

/*
    for(int i = 0; i < tmp_height; i++){
        cout<<_data_->at<float>(i, index)<<" "<<tmp_data[i]<<endl;
        cout<<_label_->at<int>(i, 0)<<" "<<tmp_label[i]<<endl;
    }
*/

/*    
    for(int i = 0; i < tmp_height; i++){
        cout<<tmp_data[i]<<endl;
    }
*/

    int pow = 1;
    while(pow < tmp_height){pow <<= 1;}
    int threads = THREADS < pow ? THREADS : pow;
    bitonic_sort_with_follower(tmp_data, tmp_label, tmp_height, pow,  pow / threads, threads);

/*
    for(int i = 0; i < tmp_height; i++){
        cout<<tmp_data[i]<<endl;
    }
*/

/*
    for(int i = 0; i < tmp_height; i++){
        cout<<tmp_data[i]<<endl;
    }
*/

    int counts[class_number] = {0};
    threads = THREADS < GINI_BLOCK_SIZE_MAX ? THREADS : GINI_BLOCK_SIZE_MAX;
    //Gini_with_GPU(tmp_label, tmp_height, DEFAULT_BLOCKS, threads, counts);
    Gini_with_GPU(tmp_label, tmp_height, ((tmp_height - 1) / threads) + 1, threads, counts);
    float *ginis = Ginis_with_GPU(tmp_label, tmp_height, threads, counts);

/*
    for(int i = 0; i < tmp_height; i++){
        cout<<tmp_label[i]<<endl;
    }
*/

/*
    for(int i = 0; i < tmp_height; i++){
        cout<<ginis[i]<<endl;
    }
*/
    
    int minimum_index = min_element(ginis, ginis + tmp_height) - ginis;
    *return_threshold = tmp_data[minimum_index];
    float minimum = ginis[minimum_index];
    *gain = ginis[tmp_height - 1] - minimum;

/*
    cout<<"minimum_index:"<<minimum_index<<endl;
*/

    return minimum;
}

void FK::Tree::add_leaf(FK::Tree::TreeNode *root, cv::Mat *_label_){
    int counts[class_number] = {0};
    int tmp_height = _label_->size().height;
    root->is_leaf = true;
    for(int i = 0; i < tmp_height; i++){
        _label_->at<int>(i, 0);
        counts[_label_->at<int>(i, 0)]++;
    }
    int maximum_index = max_element(counts, counts + class_number) - counts;
    root->classification = maximum_index;
}

FK::Tree::TreeNode* FK::Tree::BuildTree_with_GPU(int depth, cv::Mat *_data_, cv::Mat *_label_, bool with_cpu){
    if(depth > MAX_DEPTH){
        return NULL;
    }

    int tmp_height = _label_->size().height;

    if(USE_CPU(tmp_height, with_cpu)){
        return BuildTree(depth, _data_, _label_);
    }

    FK::Tree::TreeNode *root = (FK::Tree::TreeNode *)malloc(sizeof(FK::Tree::TreeNode));

    int feature_number = features.size();
    int feature_index = 0;
    float minimum = 1.0;
    float return_threshold;
    float return_thres_tmp;
    float _gain_;
    float gain_tmp;

    for(int i = 0; i < feature_number; i++){
        float mini_tmp = get_minimum_gini_with_GPU(_data_, _label_, i, &return_thres_tmp, &gain_tmp);
        if(mini_tmp < minimum){
            minimum = mini_tmp;
            feature_index = i;
            return_threshold = return_thres_tmp;
            _gain_ = gain_tmp;
        }
    }

/*
    cout<<"depth:"<<depth<<endl;
    cout<<"gain:"<<_gain_<<endl;
    cout<<"threshold:"<<return_threshold<<endl;
*/

    root->Attribute = &features[feature_index];
    root->index = feature_index;
    root->is_leaf = false;
    root->gain = _gain_;
    root->threshold = return_threshold;

    if(_gain_ <= EPSILON){
        add_leaf(root, _label_);

        return root;
    }

    cv::Mat *first;
    cv::Mat *second;
    cv::Mat *first_label;
    cv::Mat *second_label;

/*
    cout<<*_label_<<endl;
    cout<<_data_->col(feature_index)<<endl;
*/

    split_data(feature_index, return_threshold, _data_, _label_, &first, &second, &first_label, &second_label);

    if(first->size().height == 0 || second->size().height == 0){
        add_leaf(root, _label_);

        return root;
    }

    root->lch_index = BuildTree_with_GPU(depth + 1, first, first_label, with_cpu);
    root->rch_index = BuildTree_with_GPU(depth + 1, second, second_label, with_cpu);

    if((root->lch_index == NULL) && (root->rch_index == NULL)){
        add_leaf(root, _label_);
    }

/*
    cout<<"r_depth:"<<depth<<endl;
*/

    return root;
}

void FK::Tree::Learn_with_GPU(int max_depth, float epsilon, bool with_cpu){
    MAX_DEPTH = max_depth;
    EPSILON = epsilon;
    root = BuildTree_with_GPU(1, &data, &label, with_cpu);
}
