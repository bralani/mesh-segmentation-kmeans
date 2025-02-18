#ifndef MESH_HPP
#define MESH_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <exception>
#include <string>
#include <filesystem>

/**
 * \typedef VertId
 * \brief Alias for unsigned int, representing a vertex identifier.
 */
typedef unsigned int VertId; /**< Alias for unsigned int representing vertex ID. */

/**
 * \typedef EdgeId
 * \brief Alias for unsigned int, representing an edge identifier.
 */
typedef unsigned int EdgeId; /**< Alias for unsigned int representing edge ID. */

/**
 * \typedef FaceId
 * \brief Alias for unsigned int, representing a face identifier.
 */
typedef unsigned int FaceId; /**< Alias for unsigned int representing face ID. */

#include "geometry/point/Point.hpp"
#include "geometry/mesh/Face.hpp"

/**
 * \class Mesh
 * \brief Represents a 3D mesh composed of vertices, faces, and adjacency relationships.
 *
 * The Mesh class provides methods for managing a collection of faces and vertices,
 * building face adjacency, exporting the mesh to files, and handling cluster
 * segmentation of the faces.
 */
class Mesh
{
public:
  /**
   * \brief Constructor to initialize the mesh from a file.
   *
   * This constructor takes a file path, reads the mesh data, and initializes
   * the vertices, faces, and adjacency relationships.
   *
   * \param path The file path to the mesh data.
   */
  Mesh(const std::string path);

  Mesh() = default;

  /**
   * \brief Creates a segmentation from a .segm file and returns the number of clusters.
   *
   * This method reads the provided .segm file, processes the segmentation data,
   * and assigns faces to clusters.
   *
   * \param path The path to the .segm file.
   * \return The number of clusters in the segmentation.
   */
  int createSegmentationFromSegFile(const std::filesystem::path &path);

  /**
   * \brief Builds the face adjacency relationships for the mesh.
   *
   * This method computes the adjacency list for each face in the mesh, storing
   * the adjacent faces for each face ID.
   */
  void buildFaceAdjacency();

  /**
   * \brief Overloads the output stream operator to print the mesh.
   *
   * This operator allows printing the mesh to an output stream (e.g., std::cout)
   * in a human-readable format.
   *
   * \param os The output stream.
   * \param graph The mesh object to be printed.
   * \return The output stream with the mesh data.
   */
  friend std::ostream &operator<<(std::ostream &os, const Mesh &graph);

  /**
   * \brief Gets the cluster ID of a specified face.
   *
   * This method retrieves the cluster ID assigned to a face. If the face does
   * not belong to any cluster, it returns -1.
   *
   * \param face The ID of the face.
   * \return The cluster ID of the face, or -1 if not assigned.
   */
  const int getFaceCluster(FaceId face) const;

  /**
   * \brief Sets the cluster ID for a specified face.
   *
   * This method assigns a cluster ID to a specific face.
   *
   * \param face The ID of the face.
   * \param cluster The cluster ID to be assigned to the face.
   */
  void setFaceCluster(const FaceId face, const int cluster) { faceClusters[face] = cluster; }

  /**
   * \brief Gets the list of points representing the face centroids of the mesh.
   *
   * This method returns a list of points representing the centroids (baricenters)
   * of all the faces in the mesh.
   *
   * \return A vector of 3D points representing the centroids of the mesh faces.
   */
  std::vector<Point<double, 3>> getMeshFacesPoints();

  /**
   * \brief Gets a reference to a specific face in the mesh.
   *
   * This method returns a reference to the face identified by the given FaceId.
   *
   * \param face The ID of the face.
   * \return A reference to the specified face.
   */
  Face &getFace(const FaceId face) { return meshFaces[face]; }

  /**
   * \brief Gets the number of faces in the mesh.
   *
   * This method returns the total number of faces present in the mesh.
   *
   * \return The number of faces in the mesh.
   */
  int numFaces() const { return meshFaces.size(); }

  /**
   * \brief Gets a reference to the list of vertices in the mesh.
   *
   * This method returns a reference to the list of vertices in the mesh.
   *
   * \return A reference to the mesh vertices.
   */
  std::vector<Point<double, 3>> &getVertices() { return meshVertices; }
  const

      /**
       * \brief Gets the list of faces adjacent to a given face.
       *
       * This method returns the adjacent faces for a given face identified by the
       * FaceId.
       *
       * \param id The ID of the face.
       * \return A vector of FaceIds representing the adjacent faces.
       */
      std::vector<FaceId>
      getFaceAdjacencyAt(const FaceId id) const
  {
    return faceAdjacency.at(id);
  }

  /**
   * \brief Exports the mesh to an .obj file, filtering faces by a specific cluster.
   *
   * This method exports the mesh to an .obj file, including only the faces that
   * belong to the specified cluster.
   *
   * \param filepath The path to the output .obj file.
   * \param cluster The cluster ID to filter faces.
   */
  void exportToObj(const std::string &filepath, int cluster);

  /**
   * \brief Exports the mesh to a grouped .obj file.
   *
   * This method exports the mesh to an .obj file, grouping faces according to
   * their cluster IDs.
   *
   * \param filepath The path to the output .obj file.
   */
  void exportToGroupedObj(const std::string &filepath) const;

  /**
   * \brief Gets the list of vertices in the mesh.
   *
   * This method returns the list of vertices as a vector of 3D points.
   *
   * \return A vector of 3D points representing the vertices of the mesh.
   */
  std::vector<Point<double, 3>> getMeshVertices() const { return meshVertices; }

  /**
   * \brief Gets the adjacency relationships for the faces in the mesh.
   *
   * This method returns a map where each face ID is associated with a vector
   * of adjacent face IDs.
   *
   * \return A map representing the adjacency relationships of faces.
   */
  std::unordered_map<FaceId, std::vector<FaceId>> getFaceAdjacency() const { return faceAdjacency; }

  /**
   * \brief Gets the list of faces in the mesh.
   *
   * This method returns the list of faces that constitute the mesh.
   *
   * \return A vector of faces in the mesh.
   */
  std::vector<Face> getMeshFaces() const { return meshFaces; }

  void addVertex(const Point<double, 3> &vertex);
  void addFace(const Face &face);

private:
  std::vector<Point<double, 3>> meshVertices;                    /**< List of vertices in the mesh. */
  std::vector<Face> meshFaces;                                   /**< List of faces in the mesh. */
  std::unordered_map<FaceId, int> faceClusters;                  /**< Map of face IDs to cluster IDs. */
  std::unordered_map<FaceId, std::vector<FaceId>> faceAdjacency; /**< Adjacency map for faces. */
};

#endif // MESH_HPP
