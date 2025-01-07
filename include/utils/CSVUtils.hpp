#ifndef CSVUTILS_HPP
#define CSVUTILS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include "csv.hpp"
#include "geometry/point/Point.hpp"

/**
 * Static utility class to handle reading a CSV file and creating Points from the data.
 */
class CSVUtils
{
public:
    /**
     * Static method to read a CSV file and convert its rows into Points.
     * @tparam PT The type of the coordinates.
     * @tparam PD The number of dimensions.
     * @param filepath Path to the CSV file.
     * @return A vector of Points representing the rows of the CSV file.
     * @throws std::runtime_error if the CSV file cannot be opened or contains invalid data.
     */
    template <typename PT, std::size_t PD>
    static std::vector<Point<PT, PD>> readCSV(const std::string &filepath)
    {
        std::vector<Point<PT, PD>> points; // Collection to store the Points

        try
        {
            // Create a CSV reader object
            csv::CSVReader reader(filepath);

            // Iterate through each row in the CSV
            for (csv::CSVRow &row : reader)
            {
                // Ensure the row has exactly PD columns
                if (row.size() != PD)
                {
                    throw std::runtime_error("Row does not have the correct number of dimensions: " + std::to_string(row.size()));
                }

                // Parse the row into an array of coordinates
                std::array<PT, PD> coordinates;
                for (std::size_t i = 0; i < PD; ++i)
                {
                    try
                    {
                        coordinates[i] = row[i].get<PT>(); // Parse each cell as PT
                    }
                    catch (const std::exception &e)
                    {
                        throw std::runtime_error("Non-numeric data found in CSV");
                    }
                }

                // Create a Point from the parsed coordinates and add it to the vector
                points.emplace_back(coordinates);
            }
        }
        catch (const std::exception &e)
        {
            throw std::runtime_error(std::string("Error reading CSV: ") + e.what());
        }

        return points;
    }
};

#endif
