#include <iostream>
#include "single_include/csv.hpp"
#include "CSVUtils.hpp"

using namespace std;

int main() {

    string path = "file.csv";
    
    vector<Point> points = CSVUtils::readCSV(path);

    for (const Point& point : points) {
        cout << point << '\n';
    }

    return 0;
}