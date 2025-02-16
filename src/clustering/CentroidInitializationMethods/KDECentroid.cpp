#include <cstddef>
#include "clustering/CentroidInitializationMethods/KDECentroid.hpp"

#define RAY_MIN 3
#define RANGE_MIN 9

template <typename PT, std::size_t PD>
class CentroidPoint ;

template<std::size_t PD>
class MostDistanceClass;

class Kernel; 


    // Constructor with k
    template<std::size_t PD>
    KDE<PD>::KDE(const std::vector<Point<double, PD>>& data, int k) 
        : CentroidInitMethod<double, PD>(data, k) {
        this->m_h = bandwidth_RuleOfThumb();
        this->range_number_division = std::max(RANGE_MIN, static_cast<int>(std::floor(std::cbrt(data.size()))));
        this->m_ray = RAY_MIN ; //+ (data.size() - RAY_OUT_RANGE_MIN) * (RAY_MAX - RAY_MIN) / (RAY_OUT_RANGE_MAX - RAY_OUT_RANGE_MIN);
    }

    // Constructor without k
    template<std::size_t PD>
    KDE<PD>::KDE(const std::vector<Point<double, PD>>& data) 
        : CentroidInitMethod<double, PD>(data) {
        this->m_h = bandwidth_RuleOfThumb();
        this->range_number_division = std::max(RANGE_MIN, static_cast<int>(std::floor(std::cbrt(data.size()))));
        this->m_ray = RAY_MIN; //+ (data.size() - RAY_OUT_RANGE_MIN) * (RAY_MAX - RAY_MIN) / (RAY_OUT_RANGE_MAX - RAY_OUT_RANGE_MIN);
    }
    
    // Find peaks
    template<std::size_t PD>
    void KDE<PD>::findCentroid(std::vector<CentroidPoint<double, PD>>& centroids) {

        // Generate the grid points based on the calculated ranges and steps
        std::vector<Point<double, PD>> gridPoints = generateGrid();

        // Find the peaks (local maxima) in the grid
        findLocalMaxima(gridPoints, centroids);
           
        return;
    }

    template<std::size_t PD>
    int KDE<PD>::findLocalWithoutRestriction(){
        //Necessary declaration
        std::vector<std::pair<Point<double, PD>, double>> maximaPD;
        std::vector<double> densities;

        // Generate the grid points based on the calculated ranges and steps
        std::vector<Point<double, PD>> gridPoints = generateGrid();  

        densities.resize(gridPoints.size());

        // Compute KDE density for each grid point in parallel
        #pragma omp parallel for
        for (std::size_t i = 0; i < gridPoints.size(); ++i) {
            densities[i] = this->kdeValue(gridPoints[i]);
        }

        // Thread-local storage for maxima to avoid race conditions
        std::vector<std::vector<std::pair<Point<double, PD>, double>>> threadLocalMaxima(omp_get_max_threads());

        // Identify local maxima in parallel
        #pragma omp parallel
        {
            int threadID = omp_get_thread_num(); // Get the thread ID
            auto& localMaxima = threadLocalMaxima[threadID]; // Access the thread's local maxima storage

            std::vector<std::vector<std::size_t>> neighborsParallel(omp_get_max_threads());
            std::vector<std::vector<double>> offsetsParallel(omp_get_max_threads());

            #pragma omp for
            for (size_t i = 0; i < gridPoints.size(); ++i) {
                if (isLocalMaximum(gridPoints, densities, i, neighborsParallel[omp_get_thread_num()], offsetsParallel[omp_get_thread_num()])) { // Check if the current point is a local maximum
                    localMaxima.emplace_back(gridPoints[i], densities[i]); // Add it to the local maxima
                }
            }
        }

        // Merge thread-local maxima into the global maxima vector
        for (const auto& localMaxima : threadLocalMaxima) {
            maximaPD.insert(maximaPD.end(), localMaxima.begin(), localMaxima.end());
        }

        return maximaPD.size();
    }

    /*This function allows the creation of a grid in the multidimensional space where the points to be classified reside. 
    Once the maximum and minimum values are found for each dimension, 
    each range is divided into steps. Each step in every dimension represents a point, 
    and this point will be used to calculate the density */
    template<std::size_t PD>
    std::vector<Point<double, PD>> KDE<PD>::generateGrid() {

        // Initialize the minimum and maximum limits for each dimension
        std::vector<double> minValues(PD, std::numeric_limits<double>::max());
        std::vector<double> maxValues(PD, std::numeric_limits<double>::lowest());

        // Find the minimum and maximum values in each dimension
        for (const auto& point : this->m_data) {
            for (size_t dim = 0; dim < PD; ++dim) {
                minValues[dim] = std::min(minValues[dim], point.coordinates[dim]);
                maxValues[dim] = std::max(maxValues[dim], point.coordinates[dim]);
            }
        }
        
        for (size_t dim = 0; dim < PD; ++dim) {
            m_range.push_back(maxValues[dim] - minValues[dim]);
            m_step.push_back(m_range[dim] / range_number_division); 
            m_rs.push_back(m_step[dim] * this->m_ray);
        }    

        // Compute the total number of points
        std::vector<size_t> counters(PD, 0);
        m_totalPoints = 1;
        for (size_t i = 0; i < PD; ++i) {
            m_totalPoints *= static_cast<size_t>(m_range[i] / m_step[i]) + 1;
        }

        // Initialize the grid vector to store points
        std::vector<Point<double, PD>> grid(m_totalPoints);  // Allocazione diretta

        #pragma omp parallel for
        for (size_t i = 0; i < m_totalPoints; ++i) {
            Point<double, PD> point;
            size_t index = i;

            for (size_t dim = 0; dim < PD; ++dim) {
                size_t offset = index % (static_cast<size_t>(m_range[dim] / m_step[dim]) + 1);
                point.coordinates[dim] = minValues[dim] + offset * m_step[dim];
                index /= static_cast<size_t>(m_range[dim] / m_step[dim]) + 1;
            }

            grid[i] = point;  // Scriviamo direttamente in un vettore pre-allocato
        }


        return grid; // Return the generated grid
    }


    /* Calculation of Mean and Standard Deviation.
    This function computes the mean and standard deviation of the points in the dataset
    along a specific dimension `dim`.*/
    template<std::size_t PD>
    std::pair<double, double> KDE<PD>::computeMeanAndStdDev(int dim) {
        double sum = 0.0, sumSquares = 0.0; // Initialize sum and sum of squares
        int n = (this->m_data).size(); // Number of points in the dataset

        // Iterate over all points and compute the sum and sum of squares for the given dimension
        // A possible alternative cloud be use a reduce pattern
        for (const auto& point : this->m_data) {
            double value = point.coordinates[dim]; // Extract the value in the specified dimension
            sum += value;
            sumSquares += value * value;
        }

        // Compute the mean
        double mean = sum / n;

        // Compute the variance
        double variance = (sumSquares / n) - (mean * mean);

        // Compute the standard deviation (square root of variance)
        double stdDev = sqrt(variance);

        return {mean, stdDev}; // Return the results as a pair
    }

    /* Calculation of the bandwidth matrix using the Rule of Thumb method.
    This method estimates the bandwidth for each dimension based on the 
    standard deviation of the data and the number of points. */
    template<std::size_t PD>
    Eigen::MatrixXd KDE<PD>::bandwidth_RuleOfThumb() {
        int n = (this->m_data).size(); // Number of points in the dataset
        int d = PD;          // Dimensionality of the data

        VectorXd bandwidths(d); // Vector to store the bandwidths for each dimension
        for (int i = 0; i < d; ++i) {
            // Compute the mean and standard deviation for the current dimension
            auto [mean, stdDev] = computeMeanAndStdDev(i);
            // Rule of Thumb formula: h_ii = stdDev * n^(-1/(d+4)) * (4 / d + 2)
            bandwidths[i] = stdDev * pow(n, -1.0 / (d + 4)) * pow(4.0 / (d + 2), 1.0 / (d + 4));
        }

        // Create a diagonal matrix from the squared bandwidth values
        MatrixXd bandwidthMatrix = bandwidths.array().square().matrix().asDiagonal();

        // Compute necessary components for KDE
        SelfAdjointEigenSolver<MatrixXd> solver(bandwidthMatrix);
        this->m_h_sqrt_inv = solver.operatorInverseSqrt();                  // Inverse square root of the bandwidth matrix
        this->m_h_det_sqrt = sqrt(bandwidthMatrix.determinant());           // Square root of the determinant of the bandwidth matrix
        m_transformedPoints.clear();
        for (const auto& xi : this->m_data) {
            Eigen::VectorXd transformed = m_h_sqrt_inv * pointToVector(xi);
            m_transformedPoints.push_back(transformed);
        }
        return bandwidthMatrix; // Return the bandwidth matrix
    }



    /* This defines the actual function. "x" is the independent variable, and "data" 
       represents the set of points required for the calculation.
       It simply computes the kernel density estimate (KDE) for the given input. 
        * f(x) = (1 / (n * h)) * Σ K((x - x_i) / h) for i = 1 to n
        * 
        * Where:
        * - f(x): The estimated density at point x.
        * - n: The total number of data points.
        * - h: The bandwidth (or smoothing parameter) controlling the kernel's width.
        * - x_i: The i-th data point in the dataset.
        * - K(u): The kernel function, typically a symmetric and normalized function.
     */
    template<std::size_t PD>
    double KDE<PD>::kdeValue(const Point<double, PD>& x) {
        // Check if the bandwidth matrix is initialized
        if (m_h.rows() == 0 || m_h.cols() == 0) {
            throw std::runtime_error("Bandwidth matrix is not initialized.");
        }

        // Transform the query point using the square root inverse of the bandwidth matrix
        Eigen::VectorXd transformedQuery = m_h_sqrt_inv * pointToVector(x);

        double density = 0.0; // Initialize the density value to 0

        // Iterate through all transformed points in the dataset
        Eigen::VectorXd diff(PD);
        for (size_t i = 0; i < m_transformedPoints.size(); ++i) {
            diff.noalias() = transformedQuery - m_transformedPoints[i];
            density += Kernel::gaussian(diff);
        }

        // Normalize the density using the determinant of the bandwidth matrix and the dataset size
        density /= (m_transformedPoints.size() * m_h_det_sqrt);
        
        return density; // Return the estimated density value
    }


    /* Converte un Point in un VectorXd */
    template<std::size_t PD>
    Eigen::VectorXd KDE<PD>::pointToVector(const Point<double, PD>& point) {
        VectorXd vec(PD);
        for (std::size_t i = 0; i < PD; ++i) {
            vec[i] = point.coordinates[i];
        }
        return vec;
    }



    // Find local maxima in the grid
    template<std::size_t PD>
    void KDE<PD>::findLocalMaxima(const std::vector<Point<double, PD>>& gridPoints, std::vector<CentroidPoint<double, PD>>& returnVec) {
        std::vector<std::pair<Point<double, PD>, double>> maximaPD;
        std::vector<double> densities(gridPoints.size()); 

        int countCicle = 0;
        while (true) {
            std::cout << "Counter: " << countCicle << std::endl;

            #pragma omp parallel for
            for (size_t i = 0; i < gridPoints.size(); ++i) {
                densities[i] = this->kdeValue(gridPoints[i]);
            }

            std::vector<std::vector<std::pair<Point<double, PD>, double>>> threadLocalMaxima(omp_get_max_threads());
            std::vector<std::vector<std::size_t>> neighborsParallel(omp_get_max_threads());
            std::vector<std::vector<double>> offsetsParallel(omp_get_max_threads()); 

            #pragma omp parallel
            {
                int threadID = omp_get_thread_num();
                auto& localMaxima = threadLocalMaxima[threadID];

                #pragma omp for
                for (size_t i = 0; i < gridPoints.size(); ++i) {
                    if (isLocalMaximum(gridPoints, densities, i, neighborsParallel[omp_get_thread_num()], offsetsParallel[omp_get_thread_num()])) {
                        localMaxima.emplace_back(gridPoints[i], densities[i]);
                    }
                }
            }

            for (const auto& localMaxima : threadLocalMaxima) {
                maximaPD.insert(maximaPD.end(), localMaxima.begin(), localMaxima.end());
            }

            std::cout << "\nNumber of local maxima found: " << maximaPD.size() << "\n";

            if (this->m_k != 0 && maximaPD.size() < this->m_k) {
                maximaPD.clear();

                m_h.diagonal() *= 0.40;

                SelfAdjointEigenSolver<MatrixXd> solver(m_h);
                this->m_h_sqrt_inv = solver.operatorInverseSqrt();
                this->m_h_det_sqrt = sqrt(m_h.determinant());

                m_transformedPoints.resize(this->m_data.size());
                #pragma omp parallel for
                for (size_t i = 0; i < this->m_data.size(); ++i) {
                    m_transformedPoints[i] = m_h_sqrt_inv * pointToVector(this->m_data[i]);
                }

            } else {
                if (this->m_k != 0 && maximaPD.size() > this->m_k) {
                    std::vector<Point<double, PD>> tmpCentroids;
                    tmpCentroids.reserve(maximaPD.size());
                    for (const auto& pair : maximaPD) {
                        tmpCentroids.push_back(pair.first);
                    }
                    MostDistanceClass<PD> mostDistanceClass(tmpCentroids, this->m_k);
                    mostDistanceClass.findCentroid(returnVec);
                    break;
                }

                returnVec.reserve(maximaPD.size());
                int i = 0;
                for (const auto& pair : maximaPD) {
                    returnVec.emplace_back(pair.first);
                    returnVec[i].setID(i);
                    i++;
                }
                break;
            }
            countCicle++;
        }
    }


    // Check if a point is a local maximum
    template<std::size_t PD>
    bool KDE<PD>::isLocalMaximum(const std::vector<Point<double, PD>>& gridPoints, const std::vector<double>& densities, size_t index, 
            std::vector<std::size_t>& neighborsParallel, std::vector<double>& offsetsParallel) {

        double currentDensity = densities[index];       // Get the density of the current point

        neighborsParallel.clear();
        offsetsParallel.resize(PD, 0);

        // Generate neighbors for the current point
        generateNeighbors(gridPoints, gridPoints[index], 0, neighborsParallel, offsetsParallel);

        // Compare the density of the current point with its neighbors, 
        for (const auto& neighborIndex : neighborsParallel) {
            if (densities[neighborIndex] > currentDensity) {
                return false; // Not a local maximum
            }
        }

        return true; // The point is a local maximum
    }

    template<std::size_t PD>
    void KDE<PD>::generateNeighbors(const std::vector<Point<double, PD>>& grid, const Point<double, PD>& currentPoint, int dim, std::vector<size_t>& neighbors, std::vector<double>& offsets) {
        // Base case: All dimensions processed
        if (dim == PD) {
            // Generate the neighbor by applying the current offsets
            Point<double, PD> neighbor = currentPoint;
            for (std::size_t i = 0; i < PD; ++i) {
                neighbor.coordinates[i] += offsets[i];
            }

            // Check if the neighbor exists in the grid
            for (std::size_t idx = 0; idx < grid.size(); ++idx) {
                if (grid[idx] == neighbor) {  
                    neighbors.push_back(idx); // Add neighbor index to the list
                    break;
                }
            }
            return;
        }

        // Recursive case: Iterate over all possible offsets for the current dimension
        for (double offset = -m_rs[dim]; offset <= m_rs[dim]; offset = offset + m_step[dim]) {
            offsets[dim] = offset; // Set the offset for the current dimension
            generateNeighbors(grid, currentPoint, dim + 1, neighbors, offsets); // Recurse for the next dimension
        }
    }

// Explicit template instantiation
template class KDE<3>;
template class KDE<2>;