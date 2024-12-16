#include <vector>
#include <iostream>
#include "../include/csv-parser/single_include/csv.hpp"
#include "../include/CSVUtils.hpp"
#include "../include/matplotlib-cpp/matplotlibcpp.h"

namespace plt = matplotlibcpp;

int main()
{
    try
    {
        // Path to the CSV file
        std::string path = "/app/resources/file_2d.csv";

        // Read 2D points (x, y) from the CSV file
        constexpr std::size_t DIMENSIONS = 2; // Assume 2D points (x, y)
        std::vector<Point<double, DIMENSIONS>> points;

        try
        {
            points = CSVUtils::readCSV<double, 2>(path);
        }
        catch (const std::exception &e)
        {
            std::cerr << "Failed to read CSV: " << e.what() << '\n';
            std::cerr << "Ensure the file exists at: " << path << '\n';
            return 1;
        }

        // Separate the x and y coordinates for plotting
        std::vector<double> x, y;
        for (const auto &point : points)
        {
            x.push_back(point.coordinates[0]); // x-coordinate
            y.push_back(point.coordinates[1]); // y-coordinate
        }

        // Plot the data
        plt::plot(x, y);

        // Set title and labels
        plt::title("Plot from CSV Data");
        plt::xlabel("X-axis");
        plt::ylabel("Y-axis");

        // Save the plot as an image
        plt::save("plot.png");

        // If you are running this from Docker, save to a known directory
        // plt::save("/app/output/plot.png");

        // Print points to the console
        for (const auto &point : points)
        {
            std::cout << point << '\n';
        }

        // Show the plot
        plt::show();
    }
    catch (const std::exception &e)
    {
        std::cerr << "Error: " << e.what() << '\n';
        return 1;
    }

    return 0;
}
