#pragma once
#ifndef BITONIC_SORT_H 
#define BITONIC_SORT_H

void bitonic_sort(float *values, int num_vals, int blocks, int threads);

void bitonic_sort_with_follower(float *values, int *followers, int NUM_ORIGINAL, int NUM_VALS, int BLOCKS, int THREADS);

#endif
