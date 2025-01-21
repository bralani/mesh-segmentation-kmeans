#ifndef KDE_HPP
#define KDE_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Dense>
#include <stdexcept>

#include "matplotlib-cpp/matplotlibcpp.h"
#include "geometry/point/Point.hpp"
#include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"

#define BANDWIDTHMETHODS 0
#define RANGE_NUMBER_DIVISION 20
#define NUMBER_RAY_STEP 3

namespace plt = matplotlibcpp;
using namespace Eigen;

template<std::size_t PD>
class KDE : public CentroidInitMethod<double, PD> {
public:
    KDE(std::vector<Point<double, PD>>& data, int k) : CentroidInitMethod<double, PD>(data, k) {
        this->m_h = bandwidth_RuleOfThumb();
    }

    KDE(std::vector<Point<double, PD>>& data) : CentroidInitMethod<double, PD>(data) {
        this->m_h = bandwidth_RuleOfThumb();
    }

    void findCentroid(std::vector<CentroidPoint<double, PD>>& centroids) override;
private:
    int m_bandwidthMethods;
    std::size_t m_totalPoints;
    double m_h_det_sqrt;
    Eigen::MatrixXd m_h;
    MatrixXd m_h_sqrt_inv;
    std::vector<Eigen::VectorXd> m_transformedPoints;
    std::vector<double> m_rs;
    std::vector<double> m_range;
    std::vector<double> m_step;

    //Automatic parameters setting
    void automatic_set();

    // Gaussian kernel
    double gaussianKernel(const Eigen::VectorXd& u);

    // Mean and standard deviation
    std::pair<double, double> computeMeanAndStdDev(int dim);

    // Bandwidth matrix calculation
    Eigen::MatrixXd bandwidth_RuleOfThumb();

    // KDE value calculation
    double kdeValue(const Point<double, PD>& x);

    // Convert Point to VectorXd
    Eigen::VectorXd pointToVector(const Point<double, PD>& point);

    // Generate grid
    std::vector<Point<double, PD>> generateGrid();

    // Find local maxima
    std::vector<Point<double, PD>> findLocalMaxima(const std::vector<Point<double, PD>>& gridPoints);

    // Check local maximum
    bool isLocalMaximum(const std::vector<Point<double, PD>>& gridPoints, const std::vector<double>& densities, size_t index);

    // Generate neighbors
    void generateNeighbors(const std::vector<Point<double, PD>>& grid, const Point<double, PD>& currentPoint, int dim, std::vector<size_t>& neighbors, std::vector<double>& offsets);


    void stampa_point_2d(const std::vector<Point<double, 2>>& points, const std::vector<Point<double, 2>>& peaks, const std::vector<Point<double, 2>>& grid) {
        // Vettori per le coordinate x e y dei punti della griglia
        std::vector<double> x_points, y_points;
        std::vector<double> x_grid, y_grid;
        // Vettori per le coordinate x e y dei picchi
        std::vector<double> x_peaks, y_peaks;
        //std::cout<<"POINT";
        // Estrai le coordinate dai punti
        for (const auto& point : points) {
            x_points.push_back(point.coordinates[0]); // X
            y_points.push_back(point.coordinates[1]); // Y
        }
        for (const auto& point : grid) {
            x_grid.push_back(point.coordinates[0]); // X
            y_grid.push_back(point.coordinates[1]); // Y
        }
        //std::cout<<"PEAKS";
        // Estrai le coordinate dai picchi
        for (const auto& peak : peaks) {
            x_peaks.push_back(peak.coordinates[0]); // X
            y_peaks.push_back(peak.coordinates[1]); // Y
        }

        // Traccia i punti della griglia
        plt::scatter(x_points, y_points, 10.0, {{"label", "Real points"}, {"color", "blue"}}); // Punti blu
        plt::scatter(x_grid, y_grid, 5.0, {{"label", "Grid Points"}, {"color", "green"}}); // Punti blu
        // Traccia i picchi
        plt::scatter(x_peaks, y_peaks, 20.0, {{"label", "Peaks"}, {"color", "red"}, {"marker", "*"}}); // Picchi rossi, stelle

        // Imposta i titoli degli assi
        plt::xlabel("X");
        plt::ylabel("Y");

        // Aggiungi una legenda
        plt::legend();

        // Mostra il grafico
        plt::show();
    }

};


    // Find peaks
    template<std::size_t PD>
    void KDE<PD>::findCentroid(std::vector<CentroidPoint<double, PD>>& centroids) {

        // Generate the grid points based on the calculated ranges and steps
        vector<Point<double, PD>> gridPoints = generateGrid();

        // Find the peaks (local maxima) in the grid
        std::vector<Point<double, PD>> peaks = findLocalMaxima(gridPoints);

        int i = 0;
        for (auto& p : peaks)
        {
            centroids.push_back(CentroidPoint<double, PD>(p));
            centroids[i].setID(i);
            i++;
        }
         /*   
        if(PD == 2)
            stampa_point_2d(this->m_data, peaks, gridPoints);*/

        return;
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
            m_step.push_back(m_range[dim] / RANGE_NUMBER_DIVISION); 
            m_rs.push_back(m_step[dim] * NUMBER_RAY_STEP);
        }    

        // Compute the total number of points
        std::vector<size_t> counters(PD, 0);
        m_totalPoints = 1;
        for (size_t i = 0; i < PD; ++i) {
            m_totalPoints *= static_cast<size_t>(m_range[i] / m_step[i]) + 1;
        }

        // Initialize the grid vector to store points
        std::vector<Point<double, PD>> grid;
        grid.reserve(m_totalPoints);

        for (size_t i = 0; i < m_totalPoints; ++i) {
            Point<double, PD> point;
            size_t index = i;

            // Compute the coordinates for the current point
            for (size_t dim = 0; dim < PD; ++dim) {
                size_t offset = index % (static_cast<size_t>(m_range[dim] / m_step[dim]) + 1);
                point.coordinates[dim] = minValues[dim] + offset * m_step[dim];
                index /= static_cast<size_t>(m_range[dim] / m_step[dim]) + 1;
            }

            // Add the point to the grid
            grid.push_back(point);
        }

        return grid; // Return the generated grid
    }

    /* Multivariate Gaussian Kernel Function.
    This function evaluates the Gaussian kernel at a given vector `u`.
    The Gaussian kernel is used to compute the contribution of a data point
    to the kernel density estimate (KDE). */
    template<std::size_t PD>
    double KDE<PD>::gaussianKernel(const VectorXd& u) {
        // Compute the squared norm (||u||^2)
        double norm = u.squaredNorm();
        
        // Dimensionality of the vector (number of dimensions)
        int d = PD;
        
        // Compute the normalization coefficient: 1 / (2 * PI)^(d/2)
        double coeff = 1.0 / pow(2 * M_PI, d / 2.0);
        
        // Return the Gaussian kernel value: coeff * exp(-||u||^2 / 2)
        return coeff * exp(-0.5 * norm);
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
        if (m_h.rows() == 0 || m_h.cols() == 0) {
            throw std::runtime_error("Bandwidth matrix is not initialized.");
        }
       
        Eigen::VectorXd transformedQuery = m_h_sqrt_inv * pointToVector(x);

        double density = 0.0;

        for (size_t i = 0; i < m_transformedPoints.size(); ++i) {
            const Eigen::VectorXd& transformedPoint = m_transformedPoints[i];
            Eigen::VectorXd diff = transformedQuery - transformedPoint;
            density += gaussianKernel(diff);
        }

        density /= (m_transformedPoints.size() * m_h_det_sqrt);
        return density;
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
    std::vector<Point<double, PD>> KDE<PD>::findLocalMaxima(const std::vector<Point<double, PD>>& gridPoints) {

        std::vector<std::pair<Point<double, PD>, double>> maximaPD;
        std::vector<double> densities;
        std::vector<Point<double, PD>> returnVec;

        //std::cout << "Starting findLocalMaxima...\n";
        //std::cout << "Grid points size: " << gridPoints.size() << "\n";
        int countCicle = 0;
        while (true) {
            std::cout<<"Counter: "<<countCicle<<std::endl;

            densities.resize(gridPoints.size()); 
            #pragma omp parallel for
            for (size_t i = 0; i < gridPoints.size(); ++i) {
                densities[i] = this->kdeValue(gridPoints[i]);
            }

            std::vector<std::vector<std::pair<Point<double, PD>, double>>> threadLocalMaxima(omp_get_max_threads());

            #pragma omp parallel
            {
                int threadID = omp_get_thread_num();
                auto& localMaxima = threadLocalMaxima[threadID]; 

                #pragma omp for
                for (size_t i = 0; i < gridPoints.size(); ++i) {
                    if (isLocalMaximum(gridPoints, densities, i)) {
                        localMaxima.emplace_back(gridPoints[i], densities[i]);
                    }
                }
            }

            for (const auto& localMaxima : threadLocalMaxima) {
                maximaPD.insert(maximaPD.end(), localMaxima.begin(), localMaxima.end());
            }

            //std::cout << "\nNumber of local maxima found: " << maximaPD.size() << "\n";

            // Check if we need to adjust the bandwidth matrix
            if (this->m_k != 0 && maximaPD.size() < this->m_k) {
                //std::cout << "Not enough local maxima. Adjusting bandwidth matrix...\n";

                densities.clear();
                maximaPD.clear();

                // Reduce the bandwidth
                m_h.diagonal() *= 0.85;
                //std::cout << "Bandwidth matrix reduced by 15%.\n";

                // Recompute derived parameters
                SelfAdjointEigenSolver<MatrixXd> solver(m_h);
                this->m_h_sqrt_inv = solver.operatorInverseSqrt();
                this->m_h_det_sqrt = sqrt(m_h.determinant());

                //std::cout << "Recomputing transformed points...\n";
                m_transformedPoints.clear();
                for (const auto& xi : this->m_data) {
                    Eigen::VectorXd transformed = m_h_sqrt_inv * pointToVector(xi);
                    m_transformedPoints.push_back(transformed);
                }

                //std::cout << "Bandwidth adjustment complete.\n";

            } else {
                // Too many maxima or just enough
                if (this->m_k != 0 && maximaPD.size() > this->m_k) {
                    //std::cout << "Too many local maxima. Selecting top " << this->m_k << " maxima...\n";

                    std::sort(maximaPD.begin(), maximaPD.end(),
                            [](const auto& a, const auto& b) {
                                return a.second > b.second;
                            });

                    maximaPD.resize(this->m_k);
                }

                // Copy the maxima to the return vector
                //std::cout << "Finalizing local maxima selection...\n";
                for (const auto& pair : maximaPD) {
                    returnVec.push_back(pair.first);
                }
                break;
            }
            countCicle++;
        }

        //std::cout << "Local maxima search complete. Total maxima returned: " << returnVec.size() << "\n";

        return returnVec; // Return the list of local maxima
}


    // Check if a point is a local maximum
    template<std::size_t PD>
    bool KDE<PD>::isLocalMaximum(const std::vector<Point<double, PD>>& gridPoints, const std::vector<double>& densities, size_t index) {
        double currentDensity = densities[index];       // Get the density of the current point
        std::vector<size_t> neighbors;                  // Vector to store indices of neighbors
        std::vector<double> offsets(PD, 0);             // Offsets initialized to 0
        int test;

        // Generate neighbors for the current point
        //std::cout<<"Generating neighbors...they are: ";
        generateNeighbors(gridPoints, gridPoints[index], 0, neighbors, offsets);
        //std::cout<<neighbors.size()<<std::endl;

        /*std::vector<Point<double, PD>> per_stampa, per_stampa_2;
        for(auto& nei : neighbors){
            per_stampa.push_back(gridPoints[nei]);
        }
        per_stampa_2.push_back(gridPoints[index]);
        stampa_point_2d(per_stampa, per_stampa_2);
        std::cout<<"IN stampa";*/

        // Compare the density of the current point with its neighbors, 
        for (const auto& neighborIndex : neighbors) {
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

#endif
