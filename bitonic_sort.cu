#include <cuda_runtime.h>

__global__ void bitonic_sort_step(float *dev_values, int j, int k){
    unsigned int i = threadIdx.x + blockDim.x * blockIdx.x;
    unsigned int ixj = i^j;
    
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

void bitonic_sort(float *values, int NUM_VALS, int BLOCKS, int THREADS){
    float *dev_values;
    size_t size = NUM_VALS * sizeof(float);

    cudaMalloc((void**) &dev_values, size);
    cudaMemcpy(dev_values, values, size, cudaMemcpyHostToDevice);

    dim3 blocks(BLOCKS, 1);
    dim3 threads(THREADS, 1);

    for(int k = 2; k <= NUM_VALS; k <<= 1){
        for(int j = k >> 1; j > 0; j >>= 1){
            bitonic_sort_step<<<blocks, threads>>>(dev_values, j, k);
        }
    }

    cudaMemcpy(values, dev_values, size, cudaMemcpyDeviceToHost);
    cudaFree(dev_values);
}
