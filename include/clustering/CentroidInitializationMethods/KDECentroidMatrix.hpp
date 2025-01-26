#ifndef KDE3D_HPP
#define KDE3D_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Dense>
#include <stdexcept>
#include <fstream>

#include "matplotlib-cpp/matplotlibcpp.h"
#include "geometry/point/Point.hpp"
#include "clustering/CentroidInitializationMethods/KernelFunction.hpp"
#include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"
#include "clustering/CentroidInitializationMethods/MostDistantCentroids.hpp"

#define NUMBER_RAY_STEP 3
#define PDS 3
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

using Grid3D = std::vector<std::vector<std::vector<Point<double, 3>>>>;
using Densities3D = std::vector<std::vector<std::vector<double>>>;

namespace plt = matplotlibcpp;

using namespace Eigen;

class KDE3D : public CentroidInitMethod<double, 3> {
public:

    KDE3D(std::vector<Point<double, 3>>& data, int k) : CentroidInitMethod<double, 3>(data, k) {
        this->m_h = bandwidth_RuleOfThumb();
        this->range_number_division = static_cast<int>(std::floor(std::cbrt(data.size())));
    }

    KDE3D(std::vector<Point<double, 3>>& data) : CentroidInitMethod<double, 3>(data) {
        this->m_h = bandwidth_RuleOfThumb();
        this->range_number_division = static_cast<int>(std::floor(std::cbrt(data.size())));
    }

    void findCentroid(std::vector<CentroidPoint<double, 3>>& centroids) override;

private:
    int m_bandwidthMethods;
    int range_number_division;
    std::size_t m_totalPoints;
    double m_h_det_sqrt;
    Eigen::MatrixXd m_h;
    MatrixXd m_h_sqrt_inv;
    std::vector<Eigen::VectorXd> m_transformedPoints;
    std::array<double, 3> m_range;
    std::array<double, 3> m_step;
    std::array<size_t, 3> m_numPoints;

    // Mean and standard deviation
    std::pair<double, double> computeMeanAndStdDev(int dim);

    // Bandwidth matrix calculation
    Eigen::MatrixXd bandwidth_RuleOfThumb();

    // KDE3D value calculation
    double kdeValue(const Point<double, 3>& x);

    // Convert Point to VectorXd
    Eigen::VectorXd pointToVector(const Point<double, 3>& point);

    // Generate grid
    Grid3D generateGrid();

    // Find local maxima in the grid
    void findLocalMaxima(Grid3D& gridPoints, std::vector<CentroidPoint<double, PDS>>& returnVec);

    // Check if a point is a local maximum
    bool isLocalMaximum(Grid3D& gridPoints, Densities3D& densities, size_t x, size_t y, size_t z);


    double truncateToThreeDecimals(double value) {
        return std::trunc(value * 1000.0) / 1000.0;
    }
    // Funzione per esportare i punti in un file CSV
    void exportLabeledPointsToCSV(Grid3D grid3D, Densities3D densities3D) {
        std::size_t Xgrid, Ygrid, Zgrid;
        Xgrid = grid3D.size();
        Ygrid = grid3D[0].size();
        Zgrid = grid3D[0][0].size();
        
        std::ofstream file("Ex.csv");

        // Scrivi l'intestazione
        file << "x,y,z,label\n";

        for (size_t x = 0; x < Xgrid; ++x) {
            for (size_t y = 0; y < Ygrid; ++y) {
                for (size_t z = 0; z < Zgrid; ++z) {
                    file << truncateToThreeDecimals(grid3D[x][y][z].coordinates[0]) << "," 
                        << truncateToThreeDecimals(grid3D[x][y][z].coordinates[1]) << "," 
                        << truncateToThreeDecimals(grid3D[x][y][z].coordinates[2]) << "," 
                        << truncateToThreeDecimals(densities3D[x][y][z] * 10000000) << "\n";
                }
            }
        }

        file.close();
        std::cout << "Punti con etichette esportati in " << "Ex.csv" << std::endl;
    }

    void exportedMesh(std::vector<Point<double, 3>> points, std::string name_csv, std::vector<double> densities) {
                
        std::ofstream file(name_csv + ".csv");

        // Scrivi l'intestazione
        file << "x,y,z,label\n";
        int i = 0;
        for(auto& point : points){
            file << truncateToThreeDecimals(point.coordinates[0]) << "," 
                        << truncateToThreeDecimals(point.coordinates[1]) << "," 
                        << truncateToThreeDecimals(point.coordinates[2]) << "," << densities[i] << "\n";
            i++;
        }

        file.close();
        std::cout << "Punti con etichette esportati in " << "csv" << std::endl;
    }

    void exportedMesh(std::vector<Point<double, 3>> points, std::string name_csv) {
                
        std::ofstream file(name_csv + ".csv");

        // Scrivi l'intestazione
        file << "x,y,z,label\n";
        for(auto& point : points){
            file << truncateToThreeDecimals(point.coordinates[0]) << "," 
                        << truncateToThreeDecimals(point.coordinates[1]) << "," 
                        << truncateToThreeDecimals(point.coordinates[2]) << "," << 0 << "\n";
        }

        file.close();
        std::cout << "Punti con etichette esportati in " << "csv" << std::endl;
    }

};


void KDE3D::findCentroid(std::vector<CentroidPoint<double, PDS>>& centroids) {
        //CSV for print
        exportedMesh(this->m_data, "mesh");

        // Generate the grid points based on the calculated ranges and steps
        Grid3D gridPoints = generateGrid();

        // Find the peaks (local maxima) in the grid
        findLocalMaxima(gridPoints, centroids);

        std::vector<Point<double, PDS>> per_stampa;
            std::vector<double> densities_per_stamp;
            for(auto& point : centroids){
                per_stampa.push_back(point);
                densities_per_stamp.push_back(0);
            }
        exportedMesh(per_stampa, "localMaxima", densities_per_stamp);
        
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


    /* Calculation of Mean and Standard Deviation.
    This function computes the mean and standard deviation of the points in the dataset
    along a specific dimension `dim`.*/
    std::pair<double, double> KDE3D::computeMeanAndStdDev(int dim) {
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
    Eigen::MatrixXd KDE3D::bandwidth_RuleOfThumb() {
        int n = (this->m_data).size(); // Number of points in the dataset

        VectorXd bandwidths(PDS); // Vector to store the bandwidths for each dimension
        for (int i = 0; i < PDS; ++i) {
            // Compute the mean and standard deviation for the current dimension
            auto [mean, stdDev] = computeMeanAndStdDev(i);
            // Rule of Thumb formula: h_ii = stdDev * n^(-1/(d+4)) * (4 / d + 2)
            bandwidths[i] = stdDev * pow(n, -1.0 / (PDS + 4)) * pow(4.0 / (PDS + 2), 1.0 / (PDS + 4));
        }

        // Create a diagonal matrix from the squared bandwidth values
        MatrixXd bandwidthMatrix = bandwidths.array().square().matrix().asDiagonal();

        // Compute necessary components for KDE3D
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
       It simply computes the kernel density estimate (KDE3D) for the given input. 
        * f(x) = (1 / (n * h)) * Σ K((x - x_i) / h) for i = 1 to n
        * 
        * Where:
        * - f(x): The estimated density at point x.
        * - n: The total number of data points.
        * - h: The bandwidth (or smoothing parameter) controlling the kernel's width.
        * - x_i: The i-th data point in the dataset.
        * - K(u): The kernel function, typically a symmetric and normalized function.
     */
    double KDE3D::kdeValue(const Point<double, PDS>& x) {
        if (m_h.rows() == 0 || m_h.cols() == 0) {
            throw std::runtime_error("Bandwidth matrix is not initialized.");
        }
       
        Eigen::VectorXd transformedQuery = m_h_sqrt_inv * pointToVector(x);

        double density = 0.0;
        for (const auto& transformedPoint : m_transformedPoints) {
            Eigen::VectorXd diff = transformedQuery - transformedPoint;
            density += Kernel::gaussian(diff, 3);
        }

        density /= (m_transformedPoints.size() * m_h_det_sqrt);
        return density;
    }

    /* Converte un Point in un VectorXd */
    Eigen::VectorXd KDE3D::pointToVector(const Point<double, PDS>& point) {
        VectorXd vec(PDS);
        for (std::size_t i = 0; i < PDS; ++i) {
            vec[i] = point.coordinates[i];
        }
        return vec;
    }

    // Find local maxima in the grid
    void KDE3D::findLocalMaxima(Grid3D& gridPoints, std::vector<CentroidPoint<double, PDS>>& returnVec) {
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

            // Export density values and points for debugging or visualization
            exportLabeledPointsToCSV(gridPoints, densities);

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
                m_h.diagonal() *= 0.85;

                // Recompute derived parameters for the updated bandwidth
                SelfAdjointEigenSolver<MatrixXd> solver(m_h);
                this->m_h_sqrt_inv = solver.operatorInverseSqrt();
                this->m_h_det_sqrt = sqrt(m_h.determinant());

                // Update transformed points with new bandwidth
                m_transformedPoints.clear();
                for (const auto& xi : this->m_data) {
                    Eigen::VectorXd transformed = m_h_sqrt_inv * pointToVector(xi);
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
                    for (int i = 0; i < returnVec.size(); i++) {
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
    bool KDE3D::isLocalMaximum(Grid3D& gridPoints, Densities3D& densities, size_t x,size_t y,size_t z) {
        double currentDensity = densities[x][y][z];
        for (int dx = -NUMBER_RAY_STEP; dx <= NUMBER_RAY_STEP; ++dx) {
            for (int dy = -NUMBER_RAY_STEP; dy <= NUMBER_RAY_STEP; ++dy) {
                for (int dz = -NUMBER_RAY_STEP; dz <= NUMBER_RAY_STEP; ++dz) {

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

#endif
