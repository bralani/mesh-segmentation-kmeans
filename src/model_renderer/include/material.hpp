//
// Created by pisaarca on 2025/1/13.
//

#ifndef MESH_VIEWER_MATERIAL_H
#define MESH_VIEWER_MATERIAL_H

#include "shader.hpp"
#include <vector>

/**
 * @class Texture2D
 * @brief Represents a 2D texture used in materials.
 *
 * This class handles loading textures from files or raw data and
 * binding them for use in OpenGL shaders.
 */
class Texture2D
{
    friend class Material; ///< Allows Material to access private members of Texture2D.

public:
    /**
     * @brief Default constructor.
     */
    Texture2D() {}

    /**
     * @brief Constructs a Texture2D from an image file.
     * @param path The file path to the texture image.
     */
    explicit Texture2D(const std::string &path);

    /**
     * @brief Constructs a Texture2D from raw pixel data.
     * @param rawData Pointer to the raw texture data.
     * @param width The width of the texture in pixels.
     * @param height The height of the texture in pixels.
     */
    Texture2D(unsigned char *rawData, int width, int height);

    /**
     * @brief Destructor. Cleans up OpenGL texture resources.
     */
    virtual ~Texture2D();

    /**
     * @brief Activates and binds the texture for rendering.
     * @param unit The texture unit to bind the texture to.
     */
    virtual void use(int unit) const;

protected:
    const unsigned char *data = nullptr; ///< Pointer to texture data.
    GLuint glTexture;                    ///< OpenGL texture ID.
    int width, height;                   ///< Texture dimensions in pixels.
};

/**
 * @class Material
 * @brief Represents a material that combines textures and shaders.
 *
 * A material defines how an object appears by combining a shader program
 * and a set of textures. This class does not manage memory for shaders or textures.
 */
class Material
{
public:
    /**
     * @brief Default constructor.
     */
    Material() {}

    /**
     * @brief Constructs a Material with a shader program and textures.
     * @param program Pointer to the shader program.
     * @param textures A vector of texture pointers used by this material.
     */
    Material(ShaderProgram *program, std::vector<Texture2D *> textures);

    /**
     * @brief Activates the material for rendering.
     *
     * This method binds the shader program and sets up the textures
     * for rendering an object.
     */
    virtual void use() const;

protected:
    ShaderProgram *program = nullptr;  ///< Pointer to the shader program.
    std::vector<Texture2D *> textures; ///< List of textures used in the material.
};

/**
 * @class PhongMaterial
 * @brief A material implementing the Phong shading model.
 *
 * The PhongMaterial class extends Material to provide support for
 * diffuse and specular textures, as well as shininess for specular highlights.
 */
class PhongMaterial : public Material
{
public:
    /**
     * @brief Default constructor.
     */
    PhongMaterial() {}

    /**
     * @brief Constructs a PhongMaterial with shader, textures, and lighting properties.
     * @param program Pointer to the shader program.
     * @param textures A vector of texture pointers.
     * @param texDiffuse The index of the diffuse texture in the texture vector.
     * @param texSpecular The index of the specular texture in the texture vector.
     * @param shininess The shininess factor for specular highlights.
     */
    PhongMaterial(ShaderProgram *program, std::vector<Texture2D *> textures,
                  int texDiffuse, int texSpecular, float shininess);

    /**
     * @brief Activates the Phong material for rendering.
     *
     * This method binds the shader program and sets up Phong shading
     * parameters, including diffuse and specular textures.
     */
    void use() const override;

protected:
    int texDiffuse, texSpecular; ///< Texture indices for diffuse and specular components.
    float shininess;             ///< Shininess factor controlling the size of specular highlights.
};

#endif // MESH_VIEWER_MATERIAL_H
