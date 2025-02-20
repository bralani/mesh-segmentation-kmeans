//
// Created by pisaarca on 2025/1/10.
//

#ifndef MESH_VIEWER_MESH_H
#define MESH_VIEWER_MESH_H

#include <string>
#include <unordered_map>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <glad/glad.h>

#include "material.hpp"

namespace objl
{
    class Mesh;
    class Loader;
}

namespace meshviewer
{
    /**
     * @class Mesh
     * @brief Represents a 3D mesh with OpenGL rendering capabilities.
     *
     * This class stores vertex data, indices, and materials for rendering.
     */
    class Mesh
    {
    public:
        /**
         * @brief Default constructor, initializes an empty mesh.
         */
        Mesh() : indexSize(0), vertices(nullptr), indices(nullptr), glVAO(0), glVBO(0), glEBO(0), material(nullptr) {}

        /**
         * @brief Constructs a Mesh from an objl::Mesh object and a material.
         * @param mesh The OBJ Loader mesh representation.
         * @param material Pointer to the material applied to the mesh.
         */
        Mesh(const objl::Mesh &mesh, Material *material);

        /**
         * @brief Destructor. Cleans up OpenGL resources.
         */
        virtual ~Mesh();

        /**
         * @brief Draws the mesh using OpenGL.
         */
        virtual void draw() const;

        /**
         * @brief Gets the number of vertices in the mesh.
         * @return The number of vertices.
         */
        [[nodiscard]] size_t getVertexCount() const
        {
            return indexSize; // Assuming indexSize reflects the number of vertices
        }

        /**
         * @brief Retrieves a specific vertex from the mesh.
         * @param index The index of the vertex.
         * @return Pointer to the vertex data (nullptr if index is out of bounds).
         *
         * Each vertex is assumed to be composed of 3 floating-point values (x, y, z).
         */
        [[nodiscard]] float *getVertex(const size_t index) const
        {
            if (index < indexSize)
            {
                return &vertices[index * 3]; // Assuming each vertex has 3 components (x, y, z)
            }
            return nullptr; // Return nullptr if index is out of bounds
        }

    protected:
        int indexSize;              ///< Number of indices in the mesh.
        float *vertices;            ///< Pointer to the array of vertex data.
        unsigned int *indices;      ///< Pointer to the array of indices.
        GLuint glVAO, glVBO, glEBO; ///< OpenGL vertex array and buffer objects.
        Material *material;         ///< Pointer to the material applied to the mesh.
    };
}

/**
 * @class Model
 * @brief Represents a 3D model composed of multiple meshes.
 *
 * A model consists of multiple meshes and handles loading, texture management,
 * and bounding box calculations.
 */
class Model
{
public:
    /**
     * @brief Default constructor.
     */
    Model() {}

    /**
     * @brief Constructs a Model by loading it from a file.
     * @param path Path to the 3D model file.
     * @param program Pointer to the shader program used for rendering.
     */
    Model(const std::string &path, ShaderProgram *program);

    /**
     * @brief Destructor. Cleans up allocated resources.
     */
    virtual ~Model();

    /**
     * @brief Draws the model by rendering all its meshes.
     */
    virtual void draw() const;

    /**
     * @brief Gets the bounding box size of the model.
     * @return The size of the bounding box as a glm::vec3.
     */
    [[nodiscard]] virtual glm::vec3 getBoundingBoxSize() const;

    /**
     * @brief Gets the center of the model's bounding box.
     * @return The center of the bounding box as a glm::vec3.
     */
    [[nodiscard]] virtual glm::vec3 getBoundingBoxCenter() const;

protected:
    /**
     * @brief Loads a texture from a file and returns a pointer to it.
     * @param path Path to the texture file.
     * @return Pointer to the loaded texture.
     */
    Texture2D *loadTexture(const std::string &path);

    objl::Loader *loader = nullptr;                        ///< Pointer to the OBJ Loader instance.
    std::vector<meshviewer::Mesh *> meshes;                ///< List of meshes that make up the model.
    ShaderProgram *program = nullptr;                      ///< Pointer to the shader program used for rendering.
    std::unordered_map<std::string, Texture2D *> textures; ///< Texture cache.
    Texture2D *defaultTexture = nullptr;                   ///< Default texture for the model.
    std::unordered_map<std::string, Material *> materials; ///< Material cache.
};

#endif // MESH_VIEWER_MESH_H
