#ifndef KDNODE_HPP
#define KDNODE_HPP

#include "Point2.hpp"



struct KdNode {
    std::vector<Point> points;  // Punti nella cella (solo se foglia)
    Point wgtCent;              // Centroido pesato (somma dei punti)
    int count = 0;              // Numero di punti nella cella
    std::vector<double> cellMin, cellMax; // Bounding box della cella
    KdNode* left = nullptr;     // Figlio sinistro
    KdNode* right = nullptr;    // Figlio destro

    KdNode(int dimension)
        : wgtCent(dimension), cellMin(dimension), cellMax(dimension) {}

    ~KdNode() {
        delete left;
        delete right;
    }
};



#endif
