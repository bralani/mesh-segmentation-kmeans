#ifndef CSVUTILS_HPP
#define CSVUTILS_HPP

#include <iostream>
#include <vector>
#include <string>
#include <stdexcept>
#include "csv.hpp"
#include "geometry/point/Point.hpp"

/**
 * \class CSVUtils
 * \brief A static utility class for handling CSV file operations.
 * 
 * The `CSVUtils` class provides a static method to read a CSV file and convert 
 * its rows into `Point` objects. This enables easy integration of CSV data 
 * into geometric computations.
 */
class CSVUtils
{
public:
    /**
     * \brief Reads a CSV file and converts its rows into `Point` objects.
     * 
     * This static method processes a CSV file where each row corresponds to a 
     * `Point` in a multi-dimensional space. The method ensures that each row 
     * has the correct number of dimensions and converts the data into numerical 
     * values of type `PT`.
     * 
     * \tparam PT The data type of the point coordinates (e.g., `float`, `double`, `int`).
     * \tparam PD The number of dimensions of each point (e.g., 2 for 2D, 3 for 3D).
     * \param filepath The path to the CSV file.
     * \return A vector of `Point<PT, PD>` objects representing the data in the CSV file.
     * \throws std::runtime_error If the CSV file cannot be opened or contains invalid data.
     */
    template <typename PT, std::size_t PD>
    static std::vector<Point<PT, PD>> readCSV(const std::string &filepath)
    {
        std::vector<Point<PT, PD>> points; // Collection to store the parsed Points

        try
        {
            // Create a CSV reader object to process the file
            csv::CSVReader reader(filepath);

            // Iterate through each row in the CSV file
            for (csv::CSVRow &row : reader)
            {
                // Ensure the row has exactly PD columns (matches the number of dimensions)
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
                        coordinates[i] = row[i].get<PT>(); // Convert each CSV value to the required type
                    }
                    catch (const std::exception &e)
                    {
                        throw std::runtime_error("Non-numeric data found in CSV");
                    }
                }

                // Create a Point using the parsed coordinates and add it to the collection
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

#endif // CSVUTILS_HPP
