#include "matplotlibcpp.h"
#include <vector>

namespace plt = matplotlibcpp;

int main() {
    // Example data points
    std::vector<double> x = {1, 2, 3, 4, 5};
    std::vector<double> y = {2.3, 3.7, 4.1, 6.0, 8.1};

    // Plot the data
    plt::scatter(x, y);
    plt::title("2D Points");
    plt::xlabel("X-axis");
    plt::ylabel("Y-axis");

    // Show the plot
    plt::show();

    return 0;
}
