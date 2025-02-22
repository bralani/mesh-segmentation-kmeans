#ifndef FACE_HPP
#define FACE_HPP

#include <iostream>
#include <unordered_map>
#include <vector>
#include <set>
#include <exception>
#include <string>
#include <filesystem>

#include "geometry/point/Point.hpp"
#include "geometry/mesh/Mesh.hpp"

/**
 * \struct Face
 * \brief Represents a triangular face in a 3D mesh.
 * 
 * A face is defined by three vertices, and this structure stores the vertices, 
 * the computed area, the normal vector, and the baricenter (centroid) of the face.
 * The face also includes methods for calculating the area and baricenter from the 
 * provided vertices and mesh vertex data.
 */
struct Face
{
    /**
     * \brief A list of vertex indices that define the face.
     * 
     * The vertices are represented by indices into the mesh's vertex array.
     */
    std::vector<VertId> vertices;

    /**
     * \brief The area of the face.
     * 
     * The area is computed as half the magnitude of the cross product of two 
     * edge vectors of the triangle.
     */
    double area;

    /**
     * \brief The baricenter (centroid) of the face.
     * 
     * The baricenter is the average of the three vertex coordinates.
     */
    Point<double, 3> baricenter;

    /**
     * \brief The normal vector of the face.
     * 
     * The normal is computed as the cross product of two edge vectors of the 
     * triangle, and it is normalized.
     */
    Point<double, 3> normal;

    /**
     * \brief Constructor that initializes a face with three vertices and computes its area and normal.
     * 
     * This constructor takes a set of vertices, a mesh of vertices, and an ID for the face. 
     * It computes the area and normal of the face as well as its baricenter.
     * 
     * \param vertices The list of vertex indices that define the face.
     * \param meshVertices A list of all the vertices in the mesh, referenced by index.
     * \param id The unique identifier of the face.
     */
    Face(const std::vector<VertId>& vertices, const std::vector<Point<double, 3>>& meshVertices, FaceId id)
        : vertices(vertices)
    {
        // Compute the area of the face
        Point<double, 3> v0 = meshVertices[vertices[0]].coordinates;
        Point<double, 3> v1 = meshVertices[vertices[1]].coordinates;
        Point<double, 3> v2 = meshVertices[vertices[2]].coordinates;

        Point<double, 3> e1 = v1 - v0;
        Point<double, 3> e2 = v2 - v0;

        normal = e1.cross(e2);
        normal = normal / normal.norm();  // Normalize the normal

        Point<double, 3> cross = e1.cross(e2);
        area = 0.5 * cross.norm();  // Calculate the area using cross product

        // Compute the baricenter (centroid) of the face
        baricenter = (v0 + v1 + v2) / 3;
        baricenter.setID(id);  // Set the face ID on the baricenter
    }

    /**
     * \brief Default constructor for the Face struct.
     */
    Face() = default;

    /**
     * \brief Returns the area of the face.
     * 
     * This method returns the computed area of the triangular face.
     * 
     * \return The area of the face.
     */
    double getArea() const { return area; }
};

#endif
