#pragma once
#ifndef INPUT_DATA_H
#define INPUT_DATA_H

#include <string>

using namespace std;

int get_data_from_csv(string &line, float **_data_);

int get_label_from_csv(string &line, int **_label_);

#endif
