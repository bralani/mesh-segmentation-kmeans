//
// Created by pisaarca on 2025/1/10.
//

#ifndef MESH_VIEWER_MESH_H
#define MESH_VIEWER_MESH_H

#include <string>
#include <unordered_map>

#ifdef _WIN32
#include <windows.h>
#endif
#include <glad/glad.h>

#include "material.h"

namespace objl
{
    class Mesh;

    class Loader;
}

class Mesh
{
public:
    Mesh() : indexSize(0), vertices(nullptr), indices(nullptr), glVAO(0), glVBO(0), glEBO(0), material(nullptr) {}

    Mesh(const objl::Mesh &mesh, Material *material);

    virtual ~Mesh();

    virtual void draw() const;

    // Get the number of vertices (assuming it's based on the vertices array)
    [[nodiscard]] size_t getVertexCount() const
    {
        return indexSize; // Assuming indexSize reflects the number of vertices in your case
    }

    // Get a specific vertex at the given index
    // Assuming the vertices array is structured in a way that each vertex is a set of floats (e.g., x, y, z)
    [[nodiscard]] float *getVertex(const size_t index) const
    {
        if (index < indexSize)
        {
            return &vertices[index * 3]; // Assuming each vertex has 3 components (x, y, z)
        }
        return nullptr; // Return nullptr if the index is out of bounds
    }

protected:
    int indexSize;
    float *vertices;
    unsigned int *indices;
    GLuint glVAO, glVBO, glEBO;
    Material *material;
};

class Model
{
public:
    Model() {}

    Model(const std::string &path, ShaderProgram *program);

    virtual ~Model();

    virtual void draw() const;

    [[nodiscard]] virtual glm::vec3 getBoundingBoxSize() const;
    [[nodiscard]] virtual glm::vec3 getBoundingBoxCenter() const;

protected:
    Texture2D *
    loadTexture(const std::string &path);

    objl::Loader *loader = nullptr;
    std::vector<Mesh *> meshes;
    ShaderProgram *program = nullptr;
    std::unordered_map<std::string, Texture2D *> textures;
    Texture2D *defaultTexture = nullptr;
    std::unordered_map<std::string, Material *> materials;
};

#endif // MESH_VIEWER_MESH_H
