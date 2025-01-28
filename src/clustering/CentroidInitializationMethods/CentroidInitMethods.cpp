#include "clustering/CentroidInitializationMethods/CentroidInitMethods.hpp"

template <typename PT, std::size_t PD>
CentroidInitMethod<PT, PD>::CentroidInitMethod(std::vector<Point<PT, PD>>& data)
    : m_data(data), m_k(0) {}

template <typename PT, std::size_t PD>
CentroidInitMethod<PT, PD>::CentroidInitMethod(std::vector<Point<PT, PD>>& data, int k)
    : m_data(data), m_k(k) {}

template <typename PT, std::size_t PD>
void CentroidInitMethod<PT, PD>::set_k(int k) {
    m_k = k;
}

template <typename PT, std::size_t PD>
double CentroidInitMethod<PT, PD>::truncateToThreeDecimals(double value) {
    return std::trunc(value * 1000.0) / 1000.0;
}

template <typename PT, std::size_t PD>
void CentroidInitMethod<PT, PD>::exportedMesh(std::vector<Point<PT, PD>> points, std::string name_csv){
    std::ofstream file(name_csv + ".csv");

    // Write header 
    if(PD == 3)
        file << "x,y,z,label\n";
    else   
        file << "x,y,label\n";

    for (auto& point : points) {
        for (std::size_t i = 0; i < PD; ++i) {
            file << truncateToThreeDecimals(point.coordinates[i]);
            if (i < PD - 1) {
                file << ",";
            }
        }
        file << "," << 0 << "\n";
    }

    file.close();
    std::cout << "Mesh exported to " << name_csv << ".csv" << std::endl;
}

template <typename PT, std::size_t PD>
void CentroidInitMethod<PT, PD>::exportedMesh(std::vector<CentroidPoint<PT, PD>> points, std::string name_csv){
    std::vector<Point<PT, PD>> tmpPoints;
    for(CentroidPoint<PT, PD>& c : points){
        Point<PT, PD> p(c.coordinates);
        tmpPoints.push_back(p);
    }
    exportedMesh(tmpPoints, name_csv);
}


template class CentroidInitMethod<double, 2>;
template class CentroidInitMethod<double, 3>;
