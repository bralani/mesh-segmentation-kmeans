#include "geometry/point/Point.hpp"

// Default constructor
template <typename PT, std::size_t PD>
Point<PT, PD>::Point() {
    coordinates.fill(PT(0));
    id = -1;
}

// Constructor with specific array of coordinates
template <typename PT, std::size_t PD>
Point<PT, PD>::Point(const std::array<PT, PD>& coords, int id) : coordinates(coords), id(id) {}

// Constructor to initialize all coordinates with a single value
template <typename PT, std::size_t PD>
Point<PT, PD>::Point(PT value, int id) {
    coordinates.fill(value);
    this->id = id;
}

template <typename PT, std::size_t PD>
constexpr std::size_t Point<PT, PD>::getDimensions() const {
    return PD;
}

template <typename PT, std::size_t PD>
const std::array<PT, PD>& Point<PT, PD>::getValues() const {
    return coordinates;
}

template <typename PT, std::size_t PD>
void Point<PT, PD>::setValue(PT value, int idx) {
    if (idx < 0 || idx >= static_cast<int>(PD)) {
        throw std::out_of_range("Index out of bounds");
    }
    coordinates[idx] = value;
}

template <typename PT, std::size_t PD>
void Point<PT, PD>::setCentroid(std::shared_ptr<Point<PT, PD>> centroid) {
    this->centroid = centroid;
}

template <typename PT, std::size_t PD>
void Point<PT, PD>::setID(int id) {
    this->id = id;
}

template <typename PT, std::size_t PD>
bool Point<PT, PD>::operator==(const Point& other) const {
    for (std::size_t i = 0; i < PD; ++i) {
        if (std::abs(coordinates[i] - other.coordinates[i]) > 1e-6) {
            return false;
        }
    }
    return true;
}

template <typename PT, std::size_t PD>
Point<PT, PD> Point<PT, PD>::operator+(const Point<PT, PD>& other) const {
    Point<PT, PD> result;
    for (std::size_t i = 0; i < PD; ++i) {
        result.coordinates[i] = this->coordinates[i] + other.coordinates[i];
    }
    return result;
}

template <typename PT, std::size_t PD>
Point<PT, PD> Point<PT, PD>::operator-(const Point<PT, PD>& other) const {
    Point<PT, PD> result;
    for (std::size_t i = 0; i < PD; ++i) {
        result.coordinates[i] = this->coordinates[i] - other.coordinates[i];
    }
    return result;
}

template <typename PT, std::size_t PD>
Point<PT, PD> Point<PT, PD>::operator/(const Point<PT, PD>& other) const {
    Point<PT, PD> result;
    for (std::size_t i = 0; i < PD; ++i) {
        result.coordinates[i] = this->coordinates[i] / other.coordinates[i];
    }
    return result;
}

template <typename PT, std::size_t PD>
Point<PT, PD> Point<PT, PD>::vectorSum(typename std::vector<Point<PT, PD>>::iterator begin,
                                       typename std::vector<Point<PT, PD>>::iterator end) {
    Point<PT, PD> sum;
    for (auto it = begin; it != end; ++it) {
        sum = sum + *it;
    }
    return sum;
}

template <typename PT, std::size_t PD>
Point<PT, PD> Point<PT, PD>::cross(const Point<PT, PD>& other) const {
    Point<PT, PD> result;
    for (std::size_t i = 0; i < PD; ++i) {
        result.coordinates[i] = this->coordinates[(i + 1) % PD] * other.coordinates[(i + 2) % PD] - 
                                this->coordinates[(i + 2) % PD] * other.coordinates[(i + 1) % PD];
    }
    return result;
}

template <typename PT, std::size_t PD>
PT Point<PT, PD>::norm() const {
    PT sum = 0;
    for (std::size_t i = 0; i < PD; ++i) {
        sum += coordinates[i] * coordinates[i];
    }
    return std::sqrt(sum);
}

template <typename PT, std::size_t PD>
void Point<PT, PD>::print() const {
    std::cout << "(";
    for (std::size_t i = 0; i < PD; ++i) {
        std::cout << coordinates[i];
        if (i < PD - 1) std::cout << ", ";
    }
    std::cout << ")";
    if (centroid != nullptr) {
        std::cout << " -> Centroid: ";
        centroid->print();
    }
    std::cout << " Point ID: " << id;
}

template <typename PT, std::size_t PD>
void Point<PT, PD>::setCentroid(const Point<PT, PD>& point) {
        centroid = std::make_shared<Point<PT, PD>>(point);
}

// Explicit template instantiation
template class Point<double, 3>;
template class Point<double, 2>;
