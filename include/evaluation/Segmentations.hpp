#ifndef SEGMENTATIONS_HPP
#define SEGMENTATIONS_HPP

/*
 * Include files
 */
#include <fstream>
#include <vector>
#include <memory>
#include <iostream>
#include <cassert>
#include <map>
#include "Mesh.hpp"

/*
 * Definition for Segment
 */
struct Segment
{
public:
    Segment() : area(0), id(0), parent(-1) {}

    // Public getter methods
    const std::vector<FaceId> &getFaces() const { return faces; }
    double getArea() const { return area; }
    int getId() const { return id; }
    int getParent() const { return parent; }

    // Public setter methods
    void addFace(FaceId face, double faceArea)
    {
        faces.push_back(face);
        area += faceArea;
    }

    void setId(int segId) { id = segId; }
    void setParent(int parentId) { parent = parentId; }

private:
    std::vector<FaceId> faces;
    double area;
    int id;
    int parent;
};

/*
 * Definition for Segmentation
 */
class Segmentation
{
public:
    Segmentation(Mesh *mesh, int k) : area(0)
    {
        this->mesh = mesh;
        CreateSegmentation(k);
    }

    double getArea() const { return area; }

    const Segment *findSegmentById(int id) const
    {
        for (const auto &seg : segments)
        {
            if (seg.getId() == id)
                return &seg;
        }
        return nullptr;
    }

    // Get all segments
    const std::vector<Segment> &getSegments() const { return segments; }

    // Get the mesh
    Mesh *getMesh() { return mesh; }

    void setVertexValue(VertId vertex, double value)
    {
        distancesValues[vertex] = value;
    }

    double getVertexValue(VertId vertex) const
    {
        return distancesValues.at(vertex);
    }

private:
    void CreateSegmentation(int k);

    Mesh *mesh;
    std::unordered_map<VertId, double> distancesValues;
    std::vector<Segment> segments;
    double area;
};

void Segmentation::CreateSegmentation(int k)
{
    segments.resize(k);
    for (int i = 0; i < k; i++)
    {
        Segment segment;
        segment.setId(i);

        segments[i] = segment;
    }

    for (FaceId face(0); face < mesh->getMeshTopology().faceSize(); ++face)
    {
        int id = mesh->getFaceCluster(face);

        float faceArea = mesh->getMesh().area(face);
        segments[id].addFace(face, faceArea);
        area += faceArea;
    }
}

#endif // SEGMENTATIONS_HPP
