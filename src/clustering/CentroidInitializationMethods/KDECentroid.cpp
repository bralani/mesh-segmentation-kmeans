#include <cstddef>
#include "clustering/CentroidInitializationMethods/KDECentroid.hpp"


#define RAY_MIN 3
#define RANGE_MIN 9

template <typename PT, std::size_t PD>
class CentroidPoint ;

template<std::size_t PD>
class MostDistanceClass;

template <std::size_t PD>
class KDEBase;

class Kernel; 


    // Constructor with k
    template<std::size_t PD>
    KDE<PD>::KDE(const std::vector<Point<double, PD>>& data, int k) 
        : CentroidInitMethod<double, PD>(data, k) {
        this->m_h = this->bandwidth_RuleOfThumb(this->m_data);
        this->range_number_division = std::max(RANGE_MIN, static_cast<int>(std::floor(std::cbrt(data.size()))));
        this->m_ray = RAY_MIN ; //+ (data.size() - RAY_OUT_RANGE_MIN) * (RAY_MAX - RAY_MIN) / (RAY_OUT_RANGE_MAX - RAY_OUT_RANGE_MIN);
    }

    // Constructor without k
    template<std::size_t PD>
    KDE<PD>::KDE(const std::vector<Point<double, PD>>& data) 
        : CentroidInitMethod<double, PD>(data) {
        this->m_h = this->bandwidth_RuleOfThumb(this->m_data);
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

        //exportedMesh(this->m_data, "Mesh");
        //exportedMesh(centroids, "Centroids");
           
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

                this->m_h.diagonal() *= 0.40;

                SelfAdjointEigenSolver<MatrixXd> solver(this->m_h);
                this->m_h_sqrt_inv = solver.operatorInverseSqrt();
                this->m_h_det_sqrt = sqrt(this->m_h.determinant());

                this->m_transformedPoints.resize(this->m_data.size());
                #pragma omp parallel for
                for (size_t i = 0; i < this->m_data.size(); ++i) {
                    this->m_transformedPoints[i] = this->m_h_sqrt_inv * this->pointToVector(this->m_data[i]);
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