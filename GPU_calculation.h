#pragma once
#ifndef GPU_CALCULATION_H
#define GPU_CALCULATION_H

#define CLASS_NUMBER_MAX 10
#define GINI_BLOCK_SIZE_MAX 1024

void gini_calculation(int *_label_, int whole_size, int _num_, int BLOCKS, int THREADS, int *return_counts);

#endif
