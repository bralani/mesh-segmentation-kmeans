#ifndef KERNEL_HPP
#define KERNEL_HPP

#include <Eigen/Dense>
#include <cmath>

using Eigen::VectorXd;

// Definizione di M_PI se non è già definito
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

class Kernel {
public:
    // Dichiarazioni delle funzioni per i diversi kernel

    // Gaussian Kernel
    static double gaussian(const VectorXd& u);

    // Epanechnikov Kernel
    static double epanechnikov(const VectorXd& u);

    // Uniform Kernel
    static double uniform(const VectorXd& u);

    // Triangular Kernel
    static double triangular(const VectorXd& u);

    // Biweight Kernel
    static double biweight(const VectorXd& u);

    // Triweight Kernel
    static double triweight(const VectorXd& u);

    // Cosine Kernel
    static double cosine(const VectorXd& u);
};

#endif
