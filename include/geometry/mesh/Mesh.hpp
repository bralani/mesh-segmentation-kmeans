#ifndef MESH_HPP
#define MESH_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <exception>
#include <string>
#include <filesystem>

// Typedefs for Mesh identifiers (VertId, EdgeId, FaceId)
typedef unsigned int VertId; // Alias for unsigned int
typedef unsigned int EdgeId; // Alias for unsigned int
typedef unsigned int FaceId; // Alias for unsigned int

#include "geometry/point/Point.hpp"
#include "geometry/mesh/Face.hpp"

class Mesh
{
public:
  // Constructor
  Mesh(const std::string path);

  // Create a segmentation from a .segm file and returns the number of clusters
  int createSegmentationFromSegFile(const std::filesystem::path &path);

  // Build the face adjacency
  void buildFaceAdjacency();

  // Overload the output operator for printing the graph
  friend std::ostream &operator<<(std::ostream &os, const Mesh &graph);

  // Get the face cluster
  const int getFaceCluster(FaceId face) const
  {
    try
    {
      return faceClusters.at(face);
    }
    catch (const std::out_of_range &e)
    {
      return -1;
    }
  }

  // Set the face cluster
  void setFaceCluster(FaceId face, int cluster) { faceClusters[face] = cluster; }

  // Get the mesh faces as points
  std::vector<Point<double, 3>> getMeshFacesPoints()
  {
    std::vector<Point<double, 3>> faces;
    for (const auto &face : meshFaces)
    {
      faces.push_back(face.baricenter);
    }
    return faces;
  }

  Face& getFace(FaceId face)
  {
    return meshFaces[face];
  }

  int numFaces() const
  {
    return meshFaces.size();
  }

  std::vector<Point<double, 3>> &getVertices()
  {
    return meshVertices;
  }

  std::vector<FaceId> getFaceAdjacencyAt(FaceId id) const
  {
    return faceAdjacency.at(id);
  }

  void exportToObj(const std::string &filepath, int cluster);

  void exportToGroupedObj(const std::string &filepath) const;

private:
  std::vector<Point<double, 3>> meshVertices;
  std::vector<Face> meshFaces;

  std::unordered_map<FaceId, int> faceClusters;
  std::unordered_map<FaceId, std::vector<FaceId>> faceAdjacency;
};

#endif // MESH_HPP
