#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <Eigen/Dense>
#include <cmath>

using Eigen::VectorXd;
#define M_PI 3.14159265358979323846


class Kernel {
public:
    // Dichiarazioni delle funzioni per i diversi kernel

    // Gaussian Kernel
    static double gaussian(const VectorXd& u, int dimension);

    // Epanechnikov Kernel
    static double epanechnikov(const VectorXd& u, int dimension);

    // Uniform Kernel
    static double uniform(const VectorXd& u, int dimension);

    // Triangular Kernel
    static double triangular(const VectorXd& u, int dimension);

    // Biweight Kernel
    static double biweight(const VectorXd& u, int dimension);

    // Triweight Kernel
    static double triweight(const VectorXd& u, int dimension);

    // Cosine Kernel
    static double cosine(const VectorXd& u, int dimension);
};

// Gaussian Kernel
inline double Kernel::gaussian(const VectorXd& u, int dimension) {
    double norm = u.squaredNorm();
    double coeff = 1.0 / std::pow(2 * M_PI, dimension / 2.0);
    return coeff * std::exp(-0.5 * norm);
}

// Epanechnikov Kernel
inline double Kernel::epanechnikov(const VectorXd& u, int dimension) {
    double norm = u.squaredNorm();
    if (norm <= 1) {
        double coeff = 0.75; // Adjust based on dimension if needed
        return coeff * (1 - norm);
    }
    return 0.0;
}

// Uniform Kernel
inline double Kernel::uniform(const VectorXd& u, int dimension) {
    double norm = u.norm();
    if (norm <= 1) {
        return 0.5;
    }
    return 0.0;
}

// Triangular Kernel
inline double Kernel::triangular(const VectorXd& u, int dimension) {
    double norm = u.norm();
    if (norm <= 1) {
        return 1 - norm;
    }
    return 0.0;
}

// Biweight Kernel
inline double Kernel::biweight(const VectorXd& u, int dimension) {
    double norm = u.squaredNorm();
    if (norm <= 1) {
        double term = 1 - norm;
        return 15.0 / 16.0 * term * term;
    }
    return 0.0;
}

// Triweight Kernel
inline double Kernel::triweight(const VectorXd& u, int dimension) {
    double norm = u.squaredNorm();
    if (norm <= 1) {
        double term = 1 - norm;
        return 35.0 / 32.0 * term * term * term;
    }
    return 0.0;
}

// Cosine Kernel
inline double Kernel::cosine(const VectorXd& u, int dimension) {
    double norm = u.norm();
    if (norm <= 1) {
        return (M_PI / 4.0) * std::cos(M_PI * norm / 2.0);
    }
    return 0.0;
}

#endif
