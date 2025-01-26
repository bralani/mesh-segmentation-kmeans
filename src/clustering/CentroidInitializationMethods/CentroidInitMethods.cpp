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

// Instanziazioni esplicite
template class CentroidInitMethod<double, 2>;
template class CentroidInitMethod<double, 3>;
