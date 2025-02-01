#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <queue>
#include <vector>
#include <iostream>
#include <cuda_runtime.h>

#define TPB 256
#define MAX_ITER 100
#define INF_FLT 1e30f

// Kernel: assign each face to the nearest centroid using a precomputed d_distances array [K*N] 
__global__ void kMeansClusterAssignmentGeodesic(
    const float* d_distances,  // [K*N]
    int*         d_clust_assn, // [N]
    int N,
    int K
)
{
    int idx = blockIdx.x*blockDim.x + threadIdx.x;
    if(idx >= N) return;

    float minDist = INF_FLT;
    int bestC = -1;
    for(int c=0; c<K; c++){
        float dist = d_distances[c*N + idx];
        if(dist < minDist){
            minDist = dist;
            bestC   = c;
        }
    }
    d_clust_assn[idx] = bestC;
}

// Kernel: sum the baricenters of faces belonging to each cluster
__global__ void kMeansCentroidSumGeodesic(
    const float* d_faceBaricenter, // [N*dim]
    const int*   d_clust_assn,     // [N]
    float*       d_centroid_sums,  // [K*dim]
    int*         d_clust_sizes,    // [K]
    int N,
    int K,
    int dim
)
{
    int idx = blockIdx.x*blockDim.x + threadIdx.x;
    if(idx >= N) return;

    int c = d_clust_assn[idx];
    // Accumulate
    for(int d=0; d<dim; d++){
        atomicAdd(&d_centroid_sums[c*dim + d], d_faceBaricenter[idx*dim + d]);
    }
    atomicAdd(&d_clust_sizes[c], 1);
}

// Kernel: update the centroid => sum / size
__global__ void kMeansCentroidUpdateGeodesic(
    float* d_centroids, 
    const float* d_centroid_sums,
    const int*   d_clust_sizes,
    int K,
    int dim
)
{
    int c = blockIdx.x*blockDim.x + threadIdx.x;
    if(c >= K) return;

    int size = d_clust_sizes[c];
    if(size > 0){
        for(int d=0; d<dim; d++){
            d_centroids[c*dim + d] = d_centroid_sums[c*dim + d] / (float)size;
        }
    } else {
        for(int d=0; d<dim; d++){
            d_centroids[c*dim + d] = 0.f;
        }
    }
}

// CPU Dijkstra to compute geodesic distances from a single "startFace"
static void dijkstraCPU(
    int startFace,
    int N,
    const std::vector<std::vector<int>>& adjacency,
    const std::vector<float>& faceBaricenter, // [N*dim]
    int dim,
    std::vector<float>& outDist              // [N]
)
{
    outDist.assign(N, INF_FLT);
    std::vector<bool> visited(N,false);

    using PQItem = std::pair<float,int>;
    std::priority_queue<PQItem, std::vector<PQItem>, std::greater<PQItem>> pq;

    outDist[startFace] = 0.f;
    pq.push({0.f, startFace});

    while(!pq.empty()){
        auto tmp    = pq.top();
        float dist  = tmp.first;
        int faceId  = tmp.second;

        pq.pop();
        if(visited[faceId]) continue;
        visited[faceId] = true;

        // For each neighbor
        for(int neigh : adjacency[faceId]){
            float w=0.f;
            for(int d=0; d<dim; d++){
                float diff = faceBaricenter[faceId*dim + d]
                           - faceBaricenter[neigh*dim + d];
                w += diff*diff;
            }
            w = sqrtf(w);

            float nd = dist + w;
            if(nd < outDist[neigh]){
                outDist[neigh] = nd;
                pq.push({nd, neigh});
            }
        }
    }
}

static void setupGeodesicDistances(
    float* d_distances,                   // [K*N] device memory
    std::vector<float>& outDistancesHost, // [K*N], reused each iteration
    const std::vector<float>& faceBaricenter, // [N*dim]
    const std::vector<std::vector<int>>& adjacency,
    int N,
    int K,
    int dim
)
{

    for(int c=0; c<K; c++){
        int startFace = c; // or some "closestFaceId[c]"
        std::vector<float> distC(N);
        dijkstraCPU(startFace, N, adjacency, faceBaricenter, dim, distC);
        for(int f=0; f<N; f++){
            outDistancesHost[c*N + f] = distC[f];
        }
    }

    // outDistancesHost => d_distances (device)
    cudaMemcpy(d_distances,
               outDistancesHost.data(),
               K*N*sizeof(float),
               cudaMemcpyHostToDevice);
}


void kmeans_cuda_geodesic(
    int N,  // faces
    int K,  // centroids
    int dim,
    std::vector<float>& h_faceBaricenter, // [N*dim] in
    std::vector<float>& h_centroids,      // [K*dim] in/out
    std::vector<int>&   h_faceCluster,    // [N] out
    const std::vector<std::vector<int>>& adjacency,
    float threshold
)
{
    // Alloc device memory
    float* d_faceBaricenter = nullptr;
    float* d_centroids      = nullptr;
    float* d_distances      = nullptr; // [K*N]
    int*   d_clust_assn     = nullptr; // [N]

    cudaMalloc(&d_faceBaricenter, N*dim*sizeof(float));
    cudaMalloc(&d_centroids,      K*dim*sizeof(float));
    cudaMalloc(&d_distances,      K*N*sizeof(float));
    cudaMalloc(&d_clust_assn,     N*sizeof(int));

    float* d_centroid_sums = nullptr;
    cudaMalloc(&d_centroid_sums, K*dim*sizeof(float));
    int* d_clust_sizes = nullptr;
    cudaMalloc(&d_clust_sizes,   K*sizeof(int));

    // Copy initial data => device
    cudaMemcpy(d_faceBaricenter,
               h_faceBaricenter.data(),
               N*dim*sizeof(float),
               cudaMemcpyHostToDevice);
    cudaMemcpy(d_centroids,
               h_centroids.data(),
               K*dim*sizeof(float),
               cudaMemcpyHostToDevice);
    cudaMemset(d_clust_assn, 0, N*sizeof(int));

    // We'll keep oldAssignment to check convergence
    std::vector<int> oldAssignment(N, -1);

    // We'll also keep a temporary host array for d_distances
    // to store the CPU-based Dijkstra results. size = K*N
    std::vector<float> tempHostDistances(K*N, 0.f);

    bool converged = false;
    int iteration  = 0;

    while(!converged && iteration < MAX_ITER)
    {
        // "setup" => compute geodesic distances with Dijkstra + copy to device
        setupGeodesicDistances(
            d_distances,
            tempHostDistances,
            h_faceBaricenter,
            adjacency,
            N,
            K,
            dim
        );

        // cluster assignment on GPU
        {
            dim3 blocks((N + TPB -1)/TPB);
            kMeansClusterAssignmentGeodesic<<<blocks, TPB>>>(
                d_distances, 
                d_clust_assn,
                N,
                K
            );
            cudaDeviceSynchronize();
        }

        // reset sums / sizes
        cudaMemset(d_centroid_sums, 0, K*dim*sizeof(float));
        cudaMemset(d_clust_sizes,   0, K*sizeof(int));

        // sum
        {
            dim3 blocks((N + TPB -1)/TPB);
            kMeansCentroidSumGeodesic<<<blocks, TPB>>>(
                d_faceBaricenter,
                d_clust_assn,
                d_centroid_sums,
                d_clust_sizes,
                N,
                K,
                dim
            );
            cudaDeviceSynchronize();
        }

        // update
        {
            dim3 blocksC((K + TPB-1)/TPB);
            kMeansCentroidUpdateGeodesic<<<blocksC, TPB>>>(
                d_centroids,
                d_centroid_sums,
                d_clust_sizes,
                K,
                dim
            );
            cudaDeviceSynchronize();
        }

        // check how many changed => convergence
        {
            std::vector<int> tempAssign(N);
            cudaMemcpy(tempAssign.data(), d_clust_assn,
                       N*sizeof(int), cudaMemcpyDeviceToHost);

            unsigned int changed = 0;
            for(int i=0; i<N; i++){
                if(oldAssignment[i] != tempAssign[i]){
                    changed++;
                }
            }
            oldAssignment  = tempAssign;
            h_faceCluster  = tempAssign; // final assignment in host

            float ratio = (float)changed / (float)N;
            if(ratio <= threshold){
                converged = true;
            }
        }

        iteration++;
    }

    // Copy centroids back to host
    cudaMemcpy(h_centroids.data(), d_centroids,
               K*dim*sizeof(float), cudaMemcpyDeviceToHost);

    // free device memory
    cudaFree(d_faceBaricenter);
    cudaFree(d_centroids);
    cudaFree(d_distances);
    cudaFree(d_clust_assn);
    cudaFree(d_centroid_sums);
    cudaFree(d_clust_sizes);

    std::cout << "[kmeans_cuda_geodesic] finished after "
              << iteration << " iterations."
              << (converged ? " (converged)\n" : " (maxIter)\n");
}
