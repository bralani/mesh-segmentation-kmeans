#include <iostream>
#include <vector>
#include <string>
#include "single_include/csv.hpp"
#include "Point.hpp"

/**
 * Static utility class to handle reading a CSV file and creating Points from the data
 */
class CSVUtils {
public:
    /**
     * Static method to read a CSV file and convert its rows into Points
     * @param filepath Path to the CSV file
     * @return A vector of Points representing the rows of the CSV file
     * @throws std::runtime_error if the CSV file cannot be opened or contains invalid data
     */
    static std::vector<Point> readCSV(const std::string& filepath) {
        std::vector<Point> points; // Collection to store the Points

        try {
            // Create a CSV reader object
            csv::CSVReader reader(filepath);

            // Iterate through each row in the CSV
            for (csv::CSVRow& row : reader) {
                std::vector<double> values;

                // Convert each cell in the row to a double
                for (csv::CSVField& field : row) {
                    try {
                        values.push_back(field.get<double>()); // Parse as double
                    } catch (const std::exception& e) {
                        throw std::runtime_error("Non-numeric data found in CSV.");
                    }
                }

                // Create a Point using the row data (dimensions = size of the row)
                points.emplace_back(values.size(), values);
            }
        } catch (const std::exception& e) {
            throw std::runtime_error(std::string("Error reading CSV: ") + e.what());
        }

        return points;
    }
};
