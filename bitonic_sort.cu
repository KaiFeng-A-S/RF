#include <cuda_runtime.h>
#include <float.h>
#include <limits.h>
#include <iostream>

__global__ void bitonic_sort_step(float *dev_values, int j, int k){
    unsigned int i = threadIdx.x + blockDim.x * blockIdx.x;
    unsigned int ixj = i ^ j;
    
    if(ixj > i){
        if((i & k) == 0){
            if(dev_values[i] > dev_values[ixj]){
                float tmp = dev_values[i];
                dev_values[i] = dev_values[ixj];
                dev_values[ixj] = tmp;
            }
        }
        else{
            if(dev_values[i] < dev_values[ixj]){
                float tmp = dev_values[i];
                dev_values[i] = dev_values[ixj];
                dev_values[ixj] = tmp;
            }
        }
    }
}

__global__ void bitonic_sort_step_with_follower(float *dev_values, int *dev_followers, int j, int k){
    unsigned int i = threadIdx.x + blockDim.x * blockIdx.x;
    unsigned int ixj = i ^ j;

    if(ixj > i){
        if((i & k) == 0){
            if(dev_values[i] > dev_values[ixj]){
                float tmp_value = dev_values[i];
                int tmp_follower = dev_followers[i];
                dev_values[i] = dev_values[ixj];
                dev_values[ixj] = tmp_value;
                dev_followers[i] = dev_followers[ixj];
                dev_followers[ixj] = tmp_follower;
            }
        }
        else{
            if(dev_values[i] < dev_values[ixj]){
                float tmp_value = dev_values[i];
                int tmp_follower = dev_followers[i];
                dev_values[i] = dev_values[ixj];
                dev_values[ixj] = tmp_value;
                dev_followers[i] = dev_followers[ixj];
                dev_followers[ixj] = tmp_follower;
            }
        }
    }
}

__global__ void max_padding(float *dev_values, int *dev_followers, int length){
    unsigned int i = threadIdx.x + blockDim.x * blockIdx.x;

    if(i >= length){
        dev_values[i] = FLT_MAX;
        dev_followers[i] = INT_MAX;
    }
}

void bitonic_sort(float *values, int NUM_VALS, int BLOCKS, int THREADS){
    float *dev_values;
    size_t size = NUM_VALS * sizeof(float);

    cudaMalloc((void **)&dev_values, size);
    cudaMemcpy(dev_values, values, size, cudaMemcpyHostToDevice);

    dim3 blocks(BLOCKS, 1);
    dim3 threads(THREADS, 1);

    for(int k = 2; k <= NUM_VALS; k <<= 1){
        for(int j = (k >> 1); j > 0; j >>= 1){
            bitonic_sort_step<<<blocks, threads>>>(dev_values, j, k);
        }
    }

    cudaMemcpy(values, dev_values, size, cudaMemcpyDeviceToHost);
    cudaFree(dev_values);
}

void bitonic_sort_with_follower(float *values, int *followers, int NUM_ORIGINAL, int NUM_VALS, int BLOCKS, int THREADS){
    float *dev_values;
    int *dev_followers;
    size_t size_values = NUM_VALS * sizeof(float);
    size_t size_followers = NUM_VALS * sizeof(int);

    cudaError_t error = cudaMalloc((void **)&dev_values, size_values);
    //std::cout<<error<<std::endl;
    error = cudaMalloc((void **)&dev_followers, size_followers);
    //std::cout<<error<<std::endl;
    error = cudaMemcpy(dev_values, values, size_values, cudaMemcpyHostToDevice);
    //std::cout<<error<<std::endl;
    error = cudaMemcpy(dev_followers, followers, size_followers, cudaMemcpyHostToDevice);
    //std::cout<<error<<std::endl;

    dim3 blocks(BLOCKS, 1);
    dim3 threads(THREADS, 1);

    max_padding<<<blocks, threads>>>(dev_values, dev_followers, NUM_ORIGINAL);

    for(int k = 2; k <= NUM_VALS; k <<= 1){
        for(int j = (k >> 1); j > 0; j >>= 1){
            bitonic_sort_step_with_follower<<<blocks, threads>>>(dev_values, dev_followers, j, k);
        }
    }

    error = cudaMemcpy(values, dev_values, size_values, cudaMemcpyDeviceToHost);
    //std::cout<<error<<std::endl;
    error = cudaMemcpy(followers, dev_followers, size_followers, cudaMemcpyDeviceToHost);
    //std::cout<<dev_values<<std::endl;
    //std::cout<<dev_followers<<std::endl;
    error = cudaFree(dev_values);
    std::cout<<error<<std::endl;
    cudaFree(dev_followers);
    //std::cout<<error<<std::endl;
}
