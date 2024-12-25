#include "Mesh.hpp"

Mesh::Mesh(const std::filesystem::path& path)
{
  std::cout << "Loading mesh from " << path << std::endl;
  MR::Mesh mesh = *MR::MeshLoad::fromAnySupportedFormat(path);
  buildGraphFromTopology(mesh.topology);
}

std::ostream &operator<<(std::ostream &os, const Mesh &graph)
{
  for (const auto &[vertex, neighbors] : graph.adjacencyList)
  {
    os << "Vertex " << vertex << " is connected to: ";
    for (const auto &neighbor : neighbors)
    {
      os << neighbor << " ";
    }
    os << std::endl;
  }
  return os;
}

const std::unordered_map<MR::VertId, std::vector<MR::VertId>>& Mesh::getGraph() const
{
  return adjacencyList;
}

void Mesh::buildGraphFromTopology(const MR::MeshTopology &meshTopology)
{
  std::unordered_map<MR::VertId, std::set<MR::VertId>> tempAdjacencyList;

  for (MR::EdgeId edge(0); edge < meshTopology.edgeSize(); ++edge)
  {
    if (!meshTopology.isLoneEdge(edge))
    {
      MR::VertId org = meshTopology.org(edge);
      MR::VertId dest = meshTopology.dest(edge);
      tempAdjacencyList[org].insert(dest);
      tempAdjacencyList[dest].insert(org);
    }
  }

  // Convert sets to vectors for adjacencyList
  for (const auto &[vertex, neighbors] : tempAdjacencyList)
  {
    adjacencyList[vertex] = std::vector<MR::VertId>(neighbors.begin(), neighbors.end());
  }
}
