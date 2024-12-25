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

class Mesh
{
public:
  // Constructor
  Mesh(const std::filesystem::path& path);

  // Overload the output operator for printing the graph
  friend std::ostream &operator<<(std::ostream &os, const Mesh &graph);

  // Get the adjacency list (graph representation)
  const std::unordered_map<MR::VertId, std::vector<MR::VertId>>& getGraph() const;

private:
  std::unordered_map<MR::VertId, std::vector<MR::VertId>> adjacencyList;

  // Build the graph from topology
  void buildGraphFromTopology(const MR::MeshTopology &meshTopology);
};

#endif // MESH_HPP
