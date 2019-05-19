#pragma once
#ifndef TREE_H
#define TREE_H

#include <iostream>
#include <string>
#include <vector>
#include <stdlib.h>
#include <float.h>
#include <opencv2/core.hpp>

using namespace std;

namespace FK{
    class Tree{
        public:
              struct TreeNode{
                  string *Attribute;
                  int index;
                  size_t _id_;
                  bool is_leaf;
                  TreeNode *lch_index;
                  TreeNode *rch_index;
                  TreeNode *parent;
                  float gain;
                  float threshold;
                  int classification;
              };
              
              TreeNode *root;
              vector<TreeNode> nodes;
              vector<string> features;
              cv::Mat data;
              cv::Mat label;
              int class_number;
              int MAX_DEPTH;
              float EPSILON;

              Tree(){

/*
                  root = (TreeNode *)malloc(sizeof(TreeNode));
                  root->Attribute = new string();
                  root->_id_ = 0;
                  root->is_leaf = false;
                  root->lch_index = NULL;
                  root->rch_index = NULL;
                  root->parent = NULL;
                  root->gain = 0.0;
                  root->threshold = DBL_MAX;
*/

                  MAX_DEPTH = -1;
              }

/*
              Tree(const Tree &tree){
                  nodes.resize(tree.nodes.size());
                  nodes.copy_from(tree.nodes);

                  return *this;
              }
*/

              void load_data(float **_input_, int *labels, int height, int width, int _number_, string *feature_names){
                  data = cv::Mat(height, width, CV_32FC1);
                  for(int i = 0; i < height; i++){
                      for(int j = 0; j < width; j++){
                          data.at<float>(i, j) = _input_[i][j];
                      }
                  }

                  label = cv::Mat(height, 1, CV_32FC1);
                  for(int i = 0; i < height; i++){
                      label.at<int>(i, 0) = labels[i];
                  }

                  class_number = _number_;

                  for(int i = 0; i < width; i++){
                      features.push_back(feature_names[i]);
                  }
              }

              void split_data(int index, float threshold, cv::Mat *original, cv::Mat *original_label, cv::Mat **first, cv::Mat **second, cv::Mat **first_label, cv::Mat **second_label);

              float Gini(cv::Mat *_data_, cv::Mat *_label_, int *return_counts);

              float get_minimum_gini(cv::Mat *_data_, cv::Mat *_label_, int index, float *return_threshlod, float *gain);

              TreeNode* BuildTree(int depth, cv::Mat *_data_, cv::Mat *_label_);

              void Learn(int max_depth, float epsilon);

              int forward(cv::Mat &_row_);

              int* Predict(float **_data_, int *_label_, int length);

              int* Predict(cv::Mat *_data_, cv::Mat *_label_);

              void get_class_number();

              TreeNode* BuildTree_with_GPU(int depth, cv::Mat *_data_, cv::Mat *_label_, bool with_cpu);

              void Learn_with_GPU(int max_depth, float epsilon, bool with_cpu);

        private:
               void reorder(cv::Mat *_original_, cv::Mat *_new_, cv::Mat *_index_, int index);
    };
}

#endif
