#include "clustering/CentroidInitializationMethods/KDECentroidMatrix.hpp"

#define RAY_MIN 3
#define RANGE_MIN 9

template <std::size_t PD>
class KDEBase;


// Constructor with k
KDE3D::KDE3D(const std::vector<Point<double, 3>>& data, int k)
    : CentroidInitMethod<double, 3>(data, k) {
    m_h = bandwidth_RuleOfThumb(this->m_data);
    this->range_number_division = static_cast<int>(std::floor(std::cbrt(data.size())));
}

// Constructor without k
KDE3D::KDE3D(const std::vector<Point<double, 3>>& data)
    : CentroidInitMethod<double, 3>(data) {
    m_h = bandwidth_RuleOfThumb(this->m_data);
    this->range_number_division = static_cast<int>(std::floor(std::cbrt(data.size())));
}

// Convert a double value to three decimal places
double KDE3D::truncateToThreeDecimals(double value) {
    return std::trunc(value * 1000.0) / 1000.0;
}


void KDE3D::findCentroid(std::vector<CentroidPoint<double, PDS>>& centroids) {
        // Generate the grid points based on the calculated ranges and steps
        Grid3D gridPoints = generateGrid();

        // Find the peaks (local maxima) in the grid
        findLocalMaxima(gridPoints, centroids);

        //exportedMesh(this->m_data, "Mesh");
        //exportedMesh(centroids, "Centroids");

        return;
}

    /*This function allows the creation of a grid in the multidimensional space where the points to be classified reside. 
    Once the maximum and minimum values are found for each dimension, 
    each range is divided into steps. Each step in every dimension represents a point, 
    and this point will be used to calculate the density */
    Grid3D KDE3D::generateGrid() {
        // Initialize min and max values for each dimension to extreme values
        std::array<double, 3> minValues = {std::numeric_limits<double>::max(), std::numeric_limits<double>::max(), std::numeric_limits<double>::max()};
        std::array<double, 3> maxValues = {std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest(), std::numeric_limits<double>::lowest()};

        // Find the minimum and maximum values for each dimension
        for (const auto& point : this->m_data) {
            for (size_t dim = 0; dim < PDS; ++dim) {
                minValues[dim] = std::min(minValues[dim], point.coordinates[dim]);
                maxValues[dim] = std::max(maxValues[dim], point.coordinates[dim]);
            }
        }

        // Compute the range, step size, and number of points for each dimension
        for (size_t dim = 0; dim < PDS; ++dim) {
            m_range[dim] = maxValues[dim] - minValues[dim]; // Range of values in the dimension
            m_step[dim] = m_range[dim] / range_number_division; // Step size based on the division factor
            m_numPoints[dim] = static_cast<size_t>(m_range[dim] / m_step[dim]) + 1; // Number of grid points in the dimension
        }

        // Initialize a 3D grid structure with the computed number of points
        Grid3D grid(
            m_numPoints[0],
            std::vector<std::vector<Point<double, PDS>>>(
                m_numPoints[1],
                std::vector<Point<double, PDS>>(m_numPoints[2])
            )
        );

        // Fill the grid with points at regular intervals
        for (size_t x = 0; x < m_numPoints[0]; ++x) {
            for (size_t y = 0; y < m_numPoints[1]; ++y) {
                for (size_t z = 0; z < m_numPoints[2]; ++z) {
                    Point<double, PDS> point;

                    // Calculate the coordinates for the current grid point
                    point.coordinates[0] = minValues[0] + x * m_step[0];
                    point.coordinates[1] = minValues[1] + y * m_step[1];
                    point.coordinates[2] = minValues[2] + z * m_step[2];

                    // Assign the point to the corresponding grid position
                    grid[x][y][z] = point;
                }
            }
        }

        return grid; // Return the 3D grid structure
    }




    // Find local maxima in the grid
    void KDE3D::findLocalMaxima(const Grid3D& gridPoints, std::vector<CentroidPoint<double, PDS>>& returnVec) {
        // Define grid dimensions
        std::size_t Xgrid, Ygrid, Zgrid;
        std::vector<std::pair<Point<double, PDS>, double>> maximaPD;

        Xgrid = gridPoints.size();
        Ygrid = gridPoints[0].size();
        Zgrid = gridPoints[0][0].size();

        // Initialize 3D density array
        Densities3D densities(Xgrid, std::vector<std::vector<double>>(Ygrid, std::vector<double>(Zgrid, 0.0)));

        int countCicle = 0; // Counter for iterations

        while (true) {
            std::cout << "Counter: " << countCicle << std::endl;

            // Compute KDE density for each grid point in parallel
            #pragma omp parallel for collapse(PDS)
            for (size_t x = 0; x < Xgrid; ++x) {
                for (size_t y = 0; y < Ygrid; ++y) {
                    for (size_t z = 0; z < Zgrid; ++z) {
                        densities[x][y][z] = kdeValue(gridPoints[x][y][z]);
                    }
                }
            }

            // Thread-local storage for local maxima
            std::vector<std::vector<std::pair<Point<double, PDS>, double>>> threadLocalMaxima(omp_get_max_threads());

            // Find local maxima in parallel
            #pragma omp parallel
            {
                int threadID = omp_get_thread_num();
                auto& localMaxima = threadLocalMaxima[threadID]; // Access thread-local maxima vector

                #pragma omp for collapse(PDS)
                for (size_t x = 0; x < Xgrid; ++x) {
                    for (size_t y = 0; y < Ygrid; ++y) {
                        for (size_t z = 0; z < Zgrid; ++z) {
                            if (isLocalMaximum(gridPoints, densities, x, y, z)) { // Check if current point is a local maximum
                                localMaxima.emplace_back(gridPoints[x][y][z], densities[x][y][z]);
                            }
                        }
                    }
                }
            }

            // Merge thread-local maxima into the global maxima vector
            for (const auto& localMaxima : threadLocalMaxima) {
                maximaPD.insert(maximaPD.end(), localMaxima.begin(), localMaxima.end());
            }

            // Check if bandwidth adjustment is necessary
            if (maximaPD.size() < this->m_k) {
                maximaPD.clear(); // Clear maxima to retry

                // Reduce the bandwidth matrix (scale diagonals by 85%)
                m_h.diagonal() *= 0.40;

                // Recompute derived parameters for the updated bandwidth
                SelfAdjointEigenSolver<MatrixXd> solver(m_h);
                m_h_sqrt_inv = solver.operatorInverseSqrt();
                m_h_det_sqrt = sqrt(m_h.determinant());

                // Update transformed points with new bandwidth
                m_transformedPoints.clear();
                for (const auto& xi : this->m_data) {
                    Eigen::VectorXd transformed = m_h_sqrt_inv * this->pointToVector(xi);
                    m_transformedPoints.push_back(transformed);
                }

            } else {
                // If too many maxima or just enough
                if (maximaPD.size() > this->m_k) {
                    std::vector<Point<double, PDS>> tmpCentroids;
                    for (const auto& pair : maximaPD) {
                        tmpCentroids.push_back(pair.first);
                    }

                    // Use distance-based method to reduce maxima to m_k
                    MostDistanceClass<PDS> mostDistanceClass(tmpCentroids, this->m_k);
                    mostDistanceClass.findCentroid(returnVec);

                    // Assign unique IDs to centroids
                    for (std::size_t i = 0; i < returnVec.size(); i++) {
                        returnVec[i].setID(i);
                    }
                    break;
                }

                // Copy local maxima directly into the return vector
                int i = 0;
                for (const auto& pair : maximaPD) {
                    returnVec.push_back(CentroidPoint<double, PDS>(pair.first));
                    returnVec[i].setID(i); // Assign IDs to centroids
                    i++;
                }
                break;
            }
            countCicle++;
        }
        return;
    }


    // Check if a point is a local maximum
    bool KDE3D::isLocalMaximum(const Grid3D& gridPoints,const Densities3D& densities, size_t x,size_t y,size_t z) {
        double currentDensity = densities[x][y][z];
        for (int dx = -RAY_MIN; dx <= RAY_MIN; ++dx) {
            for (int dy = -RAY_MIN; dy <= RAY_MIN; ++dy) {
                for (int dz = -RAY_MIN; dz <= RAY_MIN; ++dz) {

                    if (dx == 0 && dy == 0 && dz == 0) {
                        continue;
                    }

                    size_t nx = x + dx;
                    size_t ny = y + dy;
                    size_t nz = z + dz;

                    if (nx < gridPoints.size() && ny < gridPoints[nx].size() && nz < gridPoints[nx][ny].size()) {
                        if (densities[nx][ny][nz] > currentDensity) {
                            return false; 
                        }
                    }
                }
            }
        }
        return true;
    }
