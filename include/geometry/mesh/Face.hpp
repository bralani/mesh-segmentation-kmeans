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

struct Face
{
  std::vector<VertId> vertices;
  double area;
  Point<double, 3> baricenter;
  Point<double, 3> normal;


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
    normal = normal / normal.norm();

    Point<double, 3> cross = e1.cross(e2);
    area = 0.5 * cross.norm();

    // Compute the baricenter of the face
    baricenter = (v0 + v1 + v2) / 3;
    baricenter.setID(id);
  }

  double getArea() const { return area; }
};

#endif