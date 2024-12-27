#ifndef MESH_HPP
#define MESH_HPP

#include <MRMesh/MRMesh.h>
#include <MRMesh/MRMeshLoad.h>
#include <MRMesh/MRMeshSave.h>
#include <MRMesh/MRVector.h> // For VertId and EdgeId
#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <exception>
#include <string>
#include <filesystem>

// Typedefs for Mesh identifiers (VertId, EdgeId, FaceId)
typedef MR::VertId VertId;  // Alias for MR::VertId
typedef MR::EdgeId EdgeId;  // Alias for MR::EdgeId
typedef MR::FaceId FaceId;  // Alias for MR::FaceId

class Mesh
{
public:
  // Constructor
  Mesh(const std::filesystem::path& path);

  // Overload the output operator for printing the graph
  friend std::ostream &operator<<(std::ostream &os, const Mesh &graph);

  // Get the adjacency list (graph representation)
  const std::unordered_map<VertId, std::vector<VertId>>& getGraph() const;

  // Get the mesh
  const MR::Mesh& getMesh() const { return mesh; }
  
  // Get the mesh topology
  const MR::MeshTopology& getMeshTopology() const { return mesh.topology; }

  // Get the face cluster
  const int getFaceCluster(FaceId face) const { return faceClusters.at(face); }

  // Set the face cluster
  void setFaceCluster(FaceId face, int cluster) { faceClusters[face] = cluster; }

private:
  MR::Mesh mesh;
  std::unordered_map<VertId, std::vector<VertId>> adjacencyList;
  std::unordered_map<FaceId, int> faceClusters;

  // Build the graph from topology
  void buildGraph();
};

#endif // MESH_HPP
