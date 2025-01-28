#include "clustering/CentroidInitializationMethods/KernelFunction.hpp"

// Gaussian Kernel
double Kernel::gaussian(const VectorXd& u) {
    double norm = u.squaredNorm();
    double coeff = 1.0 / std::pow(2 * M_PI, u.size() / 2.0);
    return coeff * std::exp(-0.5 * norm);
}

// Epanechnikov Kernel
double Kernel::epanechnikov(const VectorXd& u) {
    double norm = u.squaredNorm();
    if (norm <= 1) {
        double coeff = 0.75; 
        return coeff * (1 - norm);
    }
    return 0.0;
}

// Uniform Kernel
double Kernel::uniform(const VectorXd& u) {
    double norm = u.norm();
    if (norm <= 1) {
        return 0.5;
    }
    return 0.0;
}

// Triangular Kernel
double Kernel::triangular(const VectorXd& u) {
    double norm = u.norm();
    if (norm <= 1) {
        return 1 - norm;
    }
    return 0.0;
}

// Biweight Kernel
double Kernel::biweight(const VectorXd& u) {
    double norm = u.squaredNorm();
    if (norm <= 1) {
        double term = 1 - norm;
        return 15.0 / 16.0 * term * term;
    }
    return 0.0;
}

// Triweight Kernel
double Kernel::triweight(const VectorXd& u) {
    double norm = u.squaredNorm();
    if (norm <= 1) {
        double term = 1 - norm;
        return 35.0 / 32.0 * term * term * term;
    }
    return 0.0;
}

// Cosine Kernel
double Kernel::cosine(const VectorXd& u) {
    double norm = u.norm();
    if (norm <= 1) {
        return (M_PI / 4.0) * std::cos(M_PI * norm / 2.0);
    }
    return 0.0;
}
