#ifndef KDE_HPP
#define KDE_HPP

#include <iostream>
#include <vector>
#include <cmath>
#include <Eigen/Dense>
#include <stdexcept>

#include "matplotlib-cpp/matplotlibcpp.h"
#include "geometry/point/Point.hpp"
namespace plt = matplotlibcpp;

using namespace std;
using namespace Eigen;



template<std::size_t PD>
class KDE : public CentroidInitMethod<double, PD> {
public:
    KDE(std::vector<Point<double, PD>>& data, int k) : CentroidInitMethod<double, PD>(data, k) {}

    KDE(std::vector<Point<double, PD>>& data) : CentroidInitMethod<double, PD>(data) {}

    void set_KDE_parameter(int bandwidthMethods, int r, int dim_ste);

    void findCentroid(std::vector<CentroidPoint<double, PD>>& centroids) override;
private:
    // Attributes
    int m_bandwidthMethods, m_r;
    Eigen::MatrixXd h_;
    //Pre-computed value fro efficienty
    MatrixXd m_h_sqrt_inv;
    double m_h_det_sqrt;
    std::vector<Eigen::VectorXd> m_transformedPoints;
    int step_dimension;

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
    void generateNeighbors(const std::vector<Point<double, PD>>& grid, const Point<double, PD>& currentPoint, int r, int dim,
                           std::vector<size_t>& neighbors, std::vector<int>& offsets);



    void stampa_point_2d(const std::vector<Point<double, PD>>& points) {
        // Vettori per le coordinate x e y di `points`
        std::vector<double> x_points, y_points;

        // Vettori per le coordinate x e y di `peaks`
        std::vector<double> x_peaks, y_peaks;

        // Estraggo le coordinate dai punti
        for (const auto& point : points) {
            x_points.push_back(point.coordinates[0]);
            y_points.push_back(point.coordinates[1]);
        }

        // Traccia i punti principali
        plt::scatter(x_points, y_points, 10.0, {{"label", "Points"}}); // Marker size = 10.0

        // Traccia i picchi (peaks) con marker più grandi

        // Imposto i titoli degli assi
        plt::xlabel("X");
        plt::ylabel("Y");

        // Aggiungi una legenda
        plt::legend();

        // Mostro il grafico
        plt::show();

        return;
    }

    void stampa_point_3d(const std::vector<Point<double, PD>>& points, std::vector<double> densities){
        // Definizione dei dati
        int i = 0;

        // Generazione della griglia di punti (x, y)
        std::vector<double> x, y, z;
        for (const auto& point : points) {
            x.push_back(point.coordinates[0]);
            y.push_back(point.coordinates[1]);
            z.push_back(1000*densities[i]);
            i++;
        }

        std::vector<double> x_points, y_points;
        for (const auto& point : this->m_data ) {
            x_points.push_back(point.coordinates[0]);
            y_points.push_back(point.coordinates[1]);
        }
        plt::scatter(x_points, y_points, 30.0, {{"label", "Points"}});

        // Creazione del grafico 2D
        plt::scatter(x, y);  // Plot dei punti
        
        // Aggiunta delle etichette con i valori z
        for (size_t i = 0; i < x.size(); ++i) {
            plt::annotate(std::to_string(z[i]), x[i], y[i]);
        }

        // Etichette degli assi e titolo
        plt::xlabel("X-axis");
        plt::ylabel("Y-axis");
        plt::title("2D Points with Z Labels");

        // Mostra il grafico
        plt::show();

        return;
    }
};


template<std::size_t PD>
void KDE<PD>::set_KDE_parameter(int bandwidthMethods, int r, int dim_ste) {
    if (bandwidthMethods != 0) {
        throw std::invalid_argument("Bandwidth method not supported, valid method: 0");
    }
    if (r <= 0) {
        throw std::invalid_argument("Parameter 'm_r' must be greater than 0.");
    }
    if (dim_ste <= 0) {
        throw std::invalid_argument("Parameter 'step_dimension' must be greater than 0.");
    }

    m_bandwidthMethods = bandwidthMethods;
    m_r = r;
    step_dimension = dim_ste;

    this->h_ = bandwidth_RuleOfThumb();
}



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
        return;
}

    /* This function creates a regular grid of points distributed across the
    multidimensional space specified by the range and step size for each dimension.
    The range can include negative values to represent data that extends below zero.*/
    template<std::size_t PD>
    std::vector<Point<double, PD>> KDE<PD>::generateGrid() {

        // Initialize min and max values for each dimension with the appropriate limits
        vector<double> minValues(PD, std::numeric_limits<double>::max());
        vector<double> maxValues(PD, std::numeric_limits<double>::lowest());

        // Find the actual minimum and maximum values in each dimension
        for (const auto& point : this->m_data) {
            for (size_t dim = 0; dim < PD; ++dim) {
                minValues[dim] = std::min(minValues[dim], point.coordinates[dim]);
                maxValues[dim] = std::max(maxValues[dim], point.coordinates[dim]);
            }
        }

        vector<double> step(PD, step_dimension); // Step size for each dimension
        vector<double> range(PD);    // Range for each dimension
        for (size_t dim = 0; dim < PD; ++dim) {
            range[dim] = maxValues[dim] - minValues[dim];
        }

        vector<Point<double, PD>> grid; // Stores the generated grid points

        // Initialize counters to track the current index in each dimension
        vector<size_t> counters(PD, 0);
        size_t totalPoints = 1; // Total number of points in the grid

        // Calculate the total number of points based on the range and step for each dimension
        for (size_t i = 0; i < PD; ++i) {
            totalPoints *= static_cast<size_t>(range[i] / step[i]) + 1;
        }

        // Generate the grid points
        for (size_t i = 0; i < totalPoints; ++i) {
            Point<double, PD> point;

            // Compute the coordinates for the current point in each dimension
            for (size_t dim = 0; dim < PD; ++dim) {
                point.coordinates[dim] = minValues[dim] + counters[dim] * step[dim];
            }

            // Add the current point to the grid
            grid.push_back(point);

            // Increment the counters for the next iteration
            for (size_t dim = 0; dim < PD; ++dim) {
                if (++counters[dim] <= static_cast<size_t>(range[dim] / step[dim])) {
                    break; // Continue to the next point in the same dimension
                }
                counters[dim] = 0; // Reset the counter for the current dimension
            }

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
        if (h_.rows() == 0 || h_.cols() == 0) {
            throw std::runtime_error("Bandwidth matrix is not initialized.");
        }
       
        Eigen::VectorXd transformedQuery = m_h_sqrt_inv * pointToVector(x);

        double density = 0.0;
        for (const auto& transformedPoint : m_transformedPoints) {
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
        
        while(1){
            for (const auto& point : gridPoints) {
                densities.push_back(this->kdeValue(point));
            }
            
            // Identify local maxima in the grid
            for (size_t i = 0; i < gridPoints.size(); ++i) {
                if (isLocalMaximum(gridPoints, densities, i)) {
                    maximaPD.emplace_back(gridPoints[i], densities[i]);
                }
            }

            if(this->m_k != 0 && maximaPD.size() < this->m_k){
                //try to reduce the band, near points will influence less the result
                densities.clear();
                maximaPD.clear();
                h_.diagonal() *= 0.85;
                //Adjust also the parameters 
                SelfAdjointEigenSolver<MatrixXd> solver(h_);
                this->m_h_sqrt_inv = solver.operatorInverseSqrt();      // Inverse square root of the bandwidth matrix
                this->m_h_det_sqrt = sqrt(h_.determinant());            // Square root of the determinant of the bandwidth matrix
                m_transformedPoints.clear();
                for (const auto& xi : this->m_data) {
                    Eigen::VectorXd transformed = m_h_sqrt_inv * pointToVector(xi);
                    m_transformedPoints.push_back(transformed);
                }
            }else{
                //there are more local maximum that necessary, catch only k-max value 
                if(his->m_k != 0 && maximaPD.size() > this->m_k){
                    std::sort(maximaPD.begin(), maximaPD.end(),
                        [](const auto& a, const auto& b) {
                            return a.second > b.second; 
                        });

                    maximaPD.resize(this->m_k);
                }
                for (const auto& pair : maximaPD) {
                    returnVec.push_back(pair.first); 
                }
                break;
            }
        }
        return returnVec; // Return the list of local maxima
    }

    // Check if a point is a local maximum
    template<std::size_t PD>
    bool KDE<PD>::isLocalMaximum(const std::vector<Point<double, PD>>& gridPoints, const std::vector<double>& densities, size_t index) {
        double currentDensity = densities[index]; // Get the density of the current point
        std::vector<size_t> neighbors;            // Vector to store indices of neighbors
        std::vector<int> offsets(PD, 0);          // Offsets initialized to 0

        // Generate neighbors for the current point
        generateNeighbors(gridPoints, gridPoints[index], m_r, 0, neighbors, offsets);

        // Compare the density of the current point with its neighbors
        for (const auto& neighborIndex : neighbors) {
            if (densities[neighborIndex] > currentDensity) {
                return false; // Not a local maximum
            }
        }

        return true; // The point is a local maximum
    }

    template<std::size_t PD>
    void KDE<PD>::generateNeighbors(const std::vector<Point<double, PD>>& grid, 
                                    const Point<double, PD>& currentPoint, 
                                    int r, 
                                    int dim,
                                    std::vector<size_t>& neighbors, 
                                    std::vector<int>& offsets) {
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
        for (int offset = -r; offset <= r; ++offset) {
            offsets[dim] = offset; // Set the offset for the current dimension
            generateNeighbors(grid, currentPoint, r, dim + 1, neighbors, offsets); // Recurse for the next dimension
        }
    }


#endif
