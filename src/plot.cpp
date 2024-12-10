#include <vector>

#include "../include/matplotlib-cpp/matplotlibcpp.h"
namespace plt = matplotlibcpp;

int main() {
    // Generate sample data
    std::vector<double> x = {1, 2, 3, 4, 5};
    std::vector<double> y = {1, 4, 9, 16, 25};

    // Plot the data
    plt::plot(x, y);

    // Set title and labels
    plt::title("Sample Plot");
    plt::xlabel("X-axis");
    plt::ylabel("Y-axis");

    // Save the plot as an image
    plt::save("plot.png");

    // Show the plot
    plt::show();

    return 0;
}
