#ifndef CENTROID_INIT_METHOD_HPP
#define CENTROID_INIT_METHOD_HPP

#include <vector>
#include <fstream>
#include <cstddef>
#include "geometry/point/Point.hpp"
#include "geometry/point/CentroidPoint.hpp"

/**
 * \class CentroidInitMethod
 * \brief A class for managing the initialization of centroids and computing cluster centroids.
 *
 * This class provides methods to initialize centroids using a dataset and compute centroids for clustering algorithms.
 *
 * \tparam PT Type of the points (e.g., float, double, etc.).
 * \tparam PD Dimension of the data points.
 */
template <typename PT, std::size_t PD>
class CentroidInitMethod {
public:
    /**
     * \brief Virtual destructor for CentroidInitMethod.
     */
    virtual ~CentroidInitMethod() = default;

    /**
     * \brief Constructor: Initializes centroids using the dataset.
     * \param data The dataset from which to initialize centroids.
     */
    explicit CentroidInitMethod(const std::vector<Point<PT, PD>>& data);

    /**
     * \brief Constructor: Initializes centroids using the dataset and the number of clusters.
     * \param data The dataset from which to initialize centroids.
     * \param k The number of centroids to initialize.
     */
    explicit CentroidInitMethod(const std::vector<Point<PT, PD>>& data, int k);

    /**
     * \brief Abstract method to find a centroid given a set of centroids.
     * \param centroids The centroids to process.
     */
    virtual void findCentroid(std::vector<CentroidPoint<PT, PD>>& centroids) = 0;

    /**
     * \brief Exports a mesh to a CSV file without densities.
     * \param points Vector of points to export.
     * \param name_csv Name of the CSV file.
     */
    static void exportedMesh(const std::vector<Point<PT, PD>>& points, const std::string& name_csv);

    /**
     * \brief Exports a mesh to a CSV file without densities.
     * \param points Vector of points to export.
     * \param name_csv Name of the CSV file.
     */
    static void exportedMesh(const std::vector<CentroidPoint<PT, PD>>& points, const std::string& name_csv);

    /**
     * \brief Truncates a double value to three decimal places.
     * \param value Value to be truncated.
     * \return Truncated value.
     */
    static double truncateToThreeDecimals(double value);
    
protected:
    std::vector<Point<PT, PD>> m_data; ///< Dataset
    std::size_t m_k = 0; ///< Number of clusters

    /**
     * \brief Setter for the number of clusters.
     * \param k The number of clusters.
     */
    void set_k(std::size_t k);
};

#endif // CENTROID_INIT_METHOD_HPP
