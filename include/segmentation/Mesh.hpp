#ifndef MESH_HPP
#define MESH_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <exception>
#include <string>
#include <filesystem>

#include "point/Point.hpp"

// Typedefs for Mesh identifiers (VertId, EdgeId, FaceId)
typedef unsigned int VertId;  // Alias for unsigned int
typedef unsigned int EdgeId;  // Alias for unsigned int
typedef unsigned int FaceId;  // Alias for unsigned int

struct Face
{
  std::vector<VertId> vertices;
  double area;
  Point<double, 3> baricenter;

  Face(const std::vector<VertId>& vertices, const std::vector<Point<double, 3>>& meshVertices, FaceId id)
      : vertices(vertices)
  {
    // Compute the area of the face
    Point<double, 3> v0 = meshVertices[vertices[0]].coordinates;
    Point<double, 3> v1 = meshVertices[vertices[1]].coordinates;
    Point<double, 3> v2 = meshVertices[vertices[2]].coordinates;

    Point<double, 3> e1 = v1 - v0;
    Point<double, 3> e2 = v2 - v0;

    Point<double, 3> cross = e1.cross(e2);
    area = 0.5 * cross.norm();

    // Compute the baricenter of the face
    baricenter = (v0 + v1 + v2) / 3;
    baricenter.setID(id);
  }

  double getArea() const { return area; }
};


class Mesh
{
public:
  // Constructor
  Mesh(const std::string path);

  // Create a segmentation from a .segm file and returns the number of clusters
  int createSegmentationFromSegFile(const std::filesystem::path& path);

  // Overload the output operator for printing the graph
  friend std::ostream &operator<<(std::ostream &os, const Mesh &graph);

  // Get the face cluster
  const int getFaceCluster(FaceId face) const { return faceClusters.at(face); }

  // Set the face cluster
  void setFaceCluster(FaceId face, int cluster) { faceClusters[face] = cluster; }

  // Get the mesh faces as points
  std::vector<Point<double, 3>> getMeshFacesPoints() {
    std::vector<Point<double, 3>> faces;
    for (const auto& face : meshFaces)
    {
      faces.push_back(face.baricenter);
    }
    return faces;
  }

  Face getFace(FaceId face) const {
    return meshFaces[face];
  }

  int numFaces() const {
    return meshFaces.size();
  }

  std::vector<Point<double, 3>>& getVertices() {
    return meshVertices;
  }

private:
  std::vector<Point<double, 3>> meshVertices;
  std::vector<Face> meshFaces;

  std::unordered_map<FaceId, int> faceClusters;
};

#endif // MESH_HPP
