#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <cuda_runtime.h>

#define TPB 256
#define MAX_ITER 100

// Compute the Euclidean distance between two points in n-dimensional space
__device__ float distance(const float *x1, const float *x2, int dim) {
    float sum = 0.0f;
    for (int i = 0; i < dim; ++i) {
        float diff = x2[i] - x1[i];
        sum += diff * diff;
    }
    return sqrtf(sum);
}

// Assign each point to the closest centroid
__global__ void kMeansClusterAssignment(const float *d_datapoints, int *d_clust_assn, const float *d_centroids, int N, int K, int dim) {
    const int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx >= N) return;

    float min_dist = INFINITY;
    int closest_centroid = -1;

    for (int c = 0; c < K; ++c) {
        float dist = distance(&d_datapoints[idx * dim], &d_centroids[c * dim], dim);
        if (dist < min_dist) {
            min_dist = dist;
            closest_centroid = c;
        }
    }

    d_clust_assn[idx] = closest_centroid;
}

// Compute the sums of datapoints assigned to each cluster
__global__ void kMeansCentroidSum(const float *d_datapoints, const int *d_clust_assn, float *d_centroid_sums, int *d_clust_sizes, int N, int K, int dim) {
    const int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx >= N) return;

    int cluster_id = d_clust_assn[idx];

    for (int d = 0; d < dim; ++d) {
        atomicAdd(&d_centroid_sums[cluster_id * dim + d], d_datapoints[idx * dim + d]);
    }
    atomicAdd(&d_clust_sizes[cluster_id], 1);
}

// Update the centroids
__global__ void kMeansCentroidUpdate(float *d_centroids, const float *d_centroid_sums, const int *d_clust_sizes, int K, int dim) {
    const int idx = blockIdx.x * blockDim.x + threadIdx.x;

    if (idx >= K) return;

    for (int d = 0; d < dim; ++d) {
        int sum_idx = idx * dim + d;
        if (d_clust_sizes[idx] > 0) {
            d_centroids[sum_idx] = d_centroid_sums[sum_idx] / d_clust_sizes[idx];
        } else {
            d_centroids[sum_idx] = 0.0f;
        }
    }
}

void kmeans_cuda(int K, int dim, int numPoints, float *points, float *centroids, int *clust_assn, float threshold) {
    float *d_datapoints = nullptr, *d_centroids = nullptr, *d_centroid_sums = nullptr;
    int *d_clust_assn = nullptr, *d_clust_sizes = nullptr;

    cudaMalloc(&d_datapoints, numPoints * dim * sizeof(float));
    cudaMalloc(&d_centroids, K * dim * sizeof(float));
    cudaMalloc(&d_centroid_sums, K * dim * sizeof(float));
    cudaMalloc(&d_clust_assn, numPoints * sizeof(int));
    cudaMalloc(&d_clust_sizes, K * sizeof(int));

    cudaMemcpy(d_datapoints, points, numPoints * dim * sizeof(float), cudaMemcpyHostToDevice);
    cudaMemcpy(d_centroids, centroids, K * dim * sizeof(float), cudaMemcpyHostToDevice);

    dim3 blocks_points((numPoints + TPB - 1) / TPB);
    dim3 blocks_clusters((K + TPB - 1) / TPB);

    for (int iter = 0; iter < MAX_ITER; ++iter) {
        kMeansClusterAssignment<<<blocks_points, TPB>>>(d_datapoints, d_clust_assn, d_centroids, numPoints, K, dim);

        cudaMemset(d_centroid_sums, 0, K * dim * sizeof(float));
        cudaMemset(d_clust_sizes, 0, K * sizeof(int));

        kMeansCentroidSum<<<blocks_points, TPB>>>(d_datapoints, d_clust_assn, d_centroid_sums, d_clust_sizes, numPoints, K, dim);
        kMeansCentroidUpdate<<<blocks_clusters, TPB>>>(d_centroids, d_centroid_sums, d_clust_sizes, K, dim);
    }

    cudaMemcpy(centroids, d_centroids, K * dim * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(clust_assn, d_clust_assn, numPoints * sizeof(int), cudaMemcpyDeviceToHost);

    cudaFree(d_datapoints);
    cudaFree(d_centroids);
    cudaFree(d_centroid_sums);
    cudaFree(d_clust_assn);
    cudaFree(d_clust_sizes);
}
