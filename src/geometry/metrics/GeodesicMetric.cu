#include <stdio.h>
#include <stdlib.h>
#include <cmath>
#include <queue>
#include <vector>
#include <iostream>
#include <cuda_runtime.h>

#define MAX_ITER 100
#define INF_FLT 1e30f

int TPB_geodesic = 256;

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

__global__ void findClosestFaceKernel(
    const float* d_faceBaricenter, // Array of face barycenters [N * dim]
    int N,                         // Total number of faces
    int dim,                       // Dimension (e.g. 3 for 3D)
    const float* d_centroid,       // Pointer to centroid coordinates (array of length dim)
    float* d_blockMinDistances,    // Output: minimum squared distance per block
    int* d_blockMinIndices         // Output: corresponding face index per block
)
{
    // Allocate shared memory: first blockDim.x floats for distances,
    // then blockDim.x ints for indices.
    extern __shared__ char sharedMem[];
    float* s_dist = reinterpret_cast<float*>(sharedMem);
    int* s_idx = reinterpret_cast<int*>(sharedMem + blockDim.x * sizeof(float));

    int tid = threadIdx.x;
    int idx = blockIdx.x * blockDim.x + tid;

    // Initialize best distance to a large value.
    float bestDist = FLT_MAX;
    int bestIdx = -1;

    if (idx < N) {
        float dist = 0.0f;
        // Compute squared Euclidean distance between the face barycenter and the centroid.
        for (int d = 0; d < dim; d++) {
            float diff = d_faceBaricenter[idx * dim + d] - d_centroid[d];
            dist += diff * diff;
        }
        bestDist = dist;
        bestIdx = idx;
    }

    s_dist[tid] = bestDist;
    s_idx[tid] = bestIdx;
    __syncthreads();

    // Reduction in shared memory.
    for (unsigned int s = blockDim.x / 2; s > 0; s >>= 1) {
        if (tid < s) {
            if (s_dist[tid + s] < s_dist[tid]) {
                s_dist[tid] = s_dist[tid + s];
                s_idx[tid] = s_idx[tid + s];
            }
        }
        __syncthreads();
    }

    // Write the block's result to global memory.
    if (tid == 0) {
        d_blockMinDistances[blockIdx.x] = s_dist[0];
        d_blockMinIndices[blockIdx.x] = s_idx[0];
    }
}

// Host function to find the closest face index for a given centroid using the GPU.
int findClosestFaceGPU(const float* d_faceBaricenter, int N, int dim, const float* d_centroid) {
    int numBlocks = (N + TPB_geodesic - 1) / TPB_geodesic;

    // Allocate memory for block-level results on the device.
    float* d_blockMinDistances;
    int* d_blockMinIndices;
    cudaMalloc(&d_blockMinDistances, numBlocks * sizeof(float));
    cudaMalloc(&d_blockMinIndices, numBlocks * sizeof(int));

    // Calculate shared memory size: each block uses TPB_geodesic * (sizeof(float) + sizeof(int)).
    size_t sharedMemSize = TPB_geodesic * (sizeof(float) + sizeof(int));

    // Launch the kernel.
    findClosestFaceKernel<<<numBlocks, TPB_geodesic, sharedMemSize>>>(
        d_faceBaricenter,
        N,
        dim,
        d_centroid,
        d_blockMinDistances,
        d_blockMinIndices
    );
    cudaDeviceSynchronize();

    // Copy the block-level results back to host.
    std::vector<float> h_blockMinDistances(numBlocks);
    std::vector<int> h_blockMinIndices(numBlocks);
    cudaMemcpy(h_blockMinDistances.data(), d_blockMinDistances, numBlocks * sizeof(float), cudaMemcpyDeviceToHost);
    cudaMemcpy(h_blockMinIndices.data(), d_blockMinIndices, numBlocks * sizeof(int), cudaMemcpyDeviceToHost);

    // Free device memory for block results.
    cudaFree(d_blockMinDistances);
    cudaFree(d_blockMinIndices);

    // Final reduction on host.
    float bestDist = FLT_MAX;
    int bestIdx = -1;
    for (int i = 0; i < numBlocks; i++) {
        if (h_blockMinDistances[i] < bestDist) {
            bestDist = h_blockMinDistances[i];
            bestIdx = h_blockMinIndices[i];
        }
    }
    return bestIdx;
}

static void setupGeodesicDistances(
    float* d_distances,                   // [K*N] device memory
    std::vector<float>& outDistancesHost, // [K*N], reused each iteration
    const float* d_faceBaricenter,        // device pointer to face barycenters
    const std::vector<float>& h_faceBaricenter, // host copy of face barycenters [N*dim]
    const std::vector<std::vector<int>>& adjacency,
    int N,
    int K,
    int dim,
    const float* d_centroids              // device pointer to centroids [K*dim]
)
{
    for (int c = 0; c < K; c++){
        // For the current centroid, compute the closest face index using the GPU.
        // d_centroids + c*dim points to the centroid 'c'.
        int startFace = findClosestFaceGPU(d_faceBaricenter, N, dim, d_centroids + c * dim);

        // Compute geodesic distances from the chosen startFace using the CPU Dijkstra.
        std::vector<float> distC(N);
        dijkstraCPU(startFace, N, adjacency, h_faceBaricenter, dim, distC);

        // Store the distances in the temporary host array.
        for (int f = 0; f < N; f++){
            outDistancesHost[c * N + f] = distC[f];
        }
    }

    // Copy the computed distances from host to device.
    cudaMemcpy(d_distances, outDistancesHost.data(), K * N * sizeof(float), cudaMemcpyHostToDevice);
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
    std::cout << "TPB_geodesic: " << TPB_geodesic << std::endl;
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
            d_faceBaricenter,  // device pointer to face barycenters
            h_faceBaricenter,  // host copy of face barycenters
            adjacency,
            N,
            K,
            dim,
            d_centroids        // device pointer to centroids
        );

        // cluster assignment on GPU
        {
            dim3 blocks((N + TPB_geodesic -1)/TPB_geodesic);
            kMeansClusterAssignmentGeodesic<<<blocks, TPB_geodesic>>>(
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
            dim3 blocks((N + TPB_geodesic -1)/TPB_geodesic);
            kMeansCentroidSumGeodesic<<<blocks, TPB_geodesic>>>(
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
            dim3 blocksC((K + TPB_geodesic-1)/TPB_geodesic);
            kMeansCentroidUpdateGeodesic<<<blocksC, TPB_geodesic>>>(
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
