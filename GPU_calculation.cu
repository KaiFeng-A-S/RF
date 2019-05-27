#include "GPU_calculation.h"
#include <iostream>

__global__ void gini_count(int *_label_, int whole_size, int _num_, int *global_output){
    unsigned int t = threadIdx.x;
    unsigned int g = t + blockIdx.x * blockDim.x;
    unsigned int grid_size = blockDim.x * gridDim.x;
    int count[CLASS_NUMBER_MAX];
    __shared__ int partial_count[GINI_BLOCK_SIZE_MAX][CLASS_NUMBER_MAX];

    for(int i = 0; i < _num_; i++){count[i] = 0;}

    for(unsigned int i = g; i < whole_size; i += grid_size){
        count[_label_[i]]++;
    }

    //printf("%d, %d\n", t, count[0]);

    for(int i = 0; i < _num_; i++){
        partial_count[t][i] = count[i];
    }

    __syncthreads();

    //printf("p:%d, %d\n", t, partial_count[t][0]);

    for(int size = (blockDim.x / 2); size > 0; size >>= 1){
        if(t < size){
            //printf("if:%d, %d\n", t, size);
            for(int i = 0; i < _num_; i++){
                partial_count[t][i] += partial_count[t + size][i];
                //printf("pc:%d, %d, %d\n", t, partial_count[t][i], partial_count[t + size][i]);
            }
        }
        __syncthreads();
    }

    //printf("pp:%d, %d\n", t, partial_count[t][0]);

    if(t == 0){
        for(int i = 0; i < _num_; i++){
            global_output[(_num_ * blockIdx.x) + i] = partial_count[0][i];
        }
    }
}

__global__ void ginis_scan(int *_label_, float *_gini_, int whole_size, int _num_, int *count){
    unsigned int t = threadIdx.x;
    //unsigned int g = t + blockIdx.x * blockDim.x;
    //unsigned int grid_size = blockDim.x * gridDim.x;
    int ROUND = ((whole_size - 1) / blockDim.x) + 1;
    int last_count[CLASS_NUMBER_MAX];

    //printf("ROUND:%d\n", ROUND);

    __shared__ int XY[GINI_BLOCK_SIZE_MAX][CLASS_NUMBER_MAX];

    for(int i = 0; i < _num_; i++){
        XY[t][i] = 0;
        last_count[i] = 0;
    }

    for(int n = 0; n < ROUND; n++){

        //printf("n:%d\n", n);

        for(int i = 0; i < _num_; i++){
            XY[t][i] = 0;
        }

        int g = t + n * blockDim.x;
        if(g < whole_size){
            XY[t][_label_[g]] = 1;
        }

/*
        for(int i = 0; i < _num_; i++){
            printf("1:%d, %d\n", t, XY[t][i]);
        }
*/

        __syncthreads();

        for(int stride = 1; stride <= (blockDim.x / 2); stride <<= 1){
            int index = (t + 1) * stride * 2 - 1;
            if(index < blockDim.x){
                for(int i = 0; i < _num_; i++){
                    XY[index][i] += XY[index - stride][i];
                }
            }
            __syncthreads();
        }
        for(int stride  = (blockDim.x / 4); stride > 0; stride >>= 1){
            int index = (t + 1) * stride * 2 - 1;
            if(index + stride < blockDim.x){
                for(int i = 0; i < _num_; i++){
                    XY[index + stride][i] += XY[index][i];
                }
            }
            __syncthreads();
        }

        for(int i = 0; i < _num_; i++){
            XY[t][i] += last_count[i];
        }

/*
        for(int i = 0; i < _num_; i++){
            printf("2:%d, %d\n", t, XY[t][i]);
        }
*/

        float purity_l = 0.0;
        for(int i = 0; i < _num_; i++){
            float class_prop = (float) XY[t][i] / (g + 1);
            purity_l += class_prop * class_prop;
        }

        //printf("d:%d\n", t);

        float purity_r = 0.0;
        if(g < (whole_size - 1)){
            //printf("w_g:%d\n", whole_size - g);
            for(int i = 0; i < _num_; i++){
                //printf("count:%d, %d, %d\n", t, i, count[i]);
                float class_prop = (float) (count[i] - XY[t][i]) / (whole_size - g - 1);
                //printf("count:%d, %d, %d\n", t, i, count[i]);
                purity_r += class_prop * class_prop;
            }
        }

        //printf("dd:%d\n", t);
        
        if(g < whole_size){
            _gini_[g] = 1 - (((float) (g + 1) / whole_size) * purity_l + ((float) (whole_size - g - 1) / whole_size) * purity_r);
        }

/*
        for(int i = 0; i < _num_; i++){
            XY[t][i] = XY[blockDim.x - 1][i];
        }
*/

/*
        for(int i = 0; i < _num_; i++){
            printf("3:%d, %d\n", t, XY[t][i]);
        }

*/
        __syncthreads();

        //printf("n:%d\n", n);

        for(int i = 0; i < _num_; i++){
            last_count[i] = XY[blockDim.x - 1][i];
        }
    }
} 

void gini_calculation(int *_label_, int whole_size, int _num_, int BLOCKS, int THREADS, int *return_counts){
    int *dev_label;
    size_t size_labels = whole_size * sizeof(int);
    cudaMalloc((void **)&dev_label, size_labels);
    cudaMemcpy(dev_label, _label_, size_labels, cudaMemcpyHostToDevice);

    int *dev_out;
    size_t size_out = _num_ * BLOCKS * sizeof(int);
    cudaMalloc((void **)&dev_out, size_out);

    dim3 blocks(BLOCKS, 1);
    dim3 threads(THREADS, 1);

    gini_count<<<blocks, threads>>>(dev_label, whole_size, _num_, dev_out);
    cudaDeviceSynchronize();

    int *out = (int *)malloc(size_out);
    cudaMemcpy(out, dev_out, size_out, cudaMemcpyDeviceToHost);
    cudaFree(dev_label);
    cudaFree(dev_out);

/*
    for(int i = 0; i < _num_ * BLOCKS; i++){
        std::cout<<out[i]<<std::endl;
    }
*/

    for(int i = 0; i < BLOCKS; i++){
        for(int j = 0; j < _num_; j++){
            return_counts[j] += out[(_num_ * i) + j];
        }
    }
}

void ginis(int *_label_, int *count, int whole_size, int _num_, int THREADS, float *return_ginis){
    int *dev_label;
    int *dev_count;
    size_t size_labels = whole_size * sizeof(int);
    size_t size_counts = _num_ * sizeof(int);
    cudaMalloc((void **)&dev_label, size_labels);
    cudaMalloc((void **)&dev_count, size_counts);
    cudaMemcpy(dev_label, _label_, size_labels, cudaMemcpyHostToDevice);
    cudaMemcpy(dev_count, count, size_counts, cudaMemcpyHostToDevice);

    float *dev_gini;
    size_t size_ginis = whole_size * sizeof(float);
    cudaMalloc((void **)&dev_gini, size_ginis);    

    dim3 blocks(1, 1);
    dim3 threads(THREADS, 1);
    ginis_scan<<<blocks, threads>>>(dev_label, dev_gini, whole_size, _num_, dev_count);
    
    cudaMemcpy(return_ginis, dev_gini, size_ginis, cudaMemcpyDeviceToHost);
    cudaFree(dev_label);
    cudaFree(dev_count);
}
