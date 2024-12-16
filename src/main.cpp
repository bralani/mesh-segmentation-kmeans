#include <vector>

#include <iostream>
#include "CSVUtils.hpp"
#include "../include/metrics.hpp"
#include "../include/KMeans.hpp"
#include "../include/CSVUtils.hpp"
#include "../include/csv-parser/single_include/csv.hpp"
#include "../include/matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

#define DIMENSION 2

using namespace std;

int main()
{

    try
    {
        string file_name;
        cout << "Enter the name of the csv file: ";
        cin >> file_name;

        // Append file_name to /app/resources/
        std::string full_path = "/app/resources/" + file_name;

        int num_clusters;
        cout << "Enter the number of clusters (parameter k): ";
        cin >> num_clusters;

        // Reading from the file
        std::vector<Point<double, DIMENSION>> points;

        try
        {
            points = CSVUtils::readCSV<double, DIMENSION>(full_path);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to read CSV: " << e.what() << '\n';
            std::cerr << "Ensure the file exists at: " << full_path << '\n';
            return 1;
        }

        DistanceMetric euclMetric = euclideanMetric;
        KMeans<double, DIMENSION> kmeans(num_clusters, points, euclMetric, 1e-4);

        // Separate the x and y coordinates for plotting
        std::vector<double> x, y;
        for (const auto &point : points)
        {
            x.push_back(point.coordinates[0]); // x-coordinate
            y.push_back(point.coordinates[1]); // y-coordinate
        }

        // Plot the data
        plt::scatter(x, y, 10);

        kmeans.fit();

        kmeans.print();

        // Set title and labels
        plt::title("Plot from CSV Data");
        plt::xlabel("X-axis");
        plt::ylabel("Y-axis");

        // Save the plot as an image
        plt::save("/app/output/plot.png");

        plt::show();

        return 0;
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }
}
