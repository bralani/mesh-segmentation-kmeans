#include <vector>

/**
 * Euclidean distance metric
 * Calculates the straight-line distance between two points
 * @param a Vector of coordinates for the first point
 * @param b Vector of coordinates for the second point
 * @return The Euclidean distance
 */
double euclideanMetric(const std::vector<double>& a, const std::vector<double>& b) {
    double sum = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += (a[i] - b[i]) * (a[i] - b[i]);
    }
    return std::sqrt(sum);
}

/**
 * Manhattan distance metric
 * Calculates the sum of the absolute differences between the coordinates of two points
 * @param a Vector of coordinates for the first point
 * @param b Vector of coordinates for the second point
 * @return The Manhattan distance
 */
double manhattanMetric(const std::vector<double>& a, const std::vector<double>& b) {
    double sum = 0;
    for (size_t i = 0; i < a.size(); ++i) {
        sum += std::abs(a[i] - b[i]);
    }
    return sum;
}