#ifndef KDNODE_HPP
#define KDNODE_HPP

#include "Point.hpp"
#include "HasWgtCent.hpp"
#include <vector>
#include <array>
#include <memory>

template <typename PT, std::size_t PD>
class KdNode : public HasWgtCent<PT, PD> {
public:
    std::array<PT, PD> cellMin;         
    std::array<PT, PD> cellMax;          
    std::unique_ptr<KdNode<PT, PD>> left = nullptr;  
    std::unique_ptr<KdNode<PT, PD>> right = nullptr; 
    std::unique_ptr<Point<PT, PD>> myPoint = nullptr;

   
    KdNode() : HasWgtCent<PT, PD>(), cellMin(), cellMax() {
        cellMin.fill(0);
        cellMax.fill(0);
    }

    KdNode(const std::array<PT, PD>& min, const std::array<PT, PD>& max)
        : HasWgtCent<PT, PD>(), cellMin(min), cellMax(max) {}


    ~KdNode() override = default;
};

#endif