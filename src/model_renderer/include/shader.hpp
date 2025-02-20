//
// Created by pisaarca on 2025/1/10.
//

#ifndef MESH_VIEWER_SHADER_H
#define MESH_VIEWER_SHADER_H

#include <string>
#include <fstream>
#include <filesystem>

#ifdef _WIN32
#define NOMINMAX
#include <windows.h>
#endif
#include <glad/glad.h>
#include <glm/glm.hpp>

#include <plog/Log.h>

/**
 * @class Shader
 * @brief Represents an OpenGL shader.
 *
 * This class encapsulates the creation, compilation, and management of
 * OpenGL shaders, such as vertex and fragment shaders.
 */
class Shader
{
    friend class ShaderProgram; ///< Allows ShaderProgram to access Shader internals.

public:
    /**
     * @brief Default constructor.
     */
    Shader() {}

    /**
     * @brief Constructs a Shader from a file.
     * @param path The file path of the shader source code.
     * @param type The type of shader (GL_VERTEX_SHADER, GL_FRAGMENT_SHADER, etc.).
     */
    explicit Shader(const std::string &path, GLenum type);

    /**
     * @brief Destructor. Cleans up OpenGL shader resources.
     */
    virtual ~Shader();

protected:
    GLuint glShader;     ///< OpenGL shader ID.
    GLenum glShaderType; ///< Type of shader (vertex, fragment, etc.).
};

/**
 * @class ShaderProgram
 * @brief Represents an OpenGL shader program.
 *
 * A shader program links multiple shaders (vertex and fragment) and allows
 * for uniform variable updates for rendering.
 */
class ShaderProgram
{
public:
    /**
     * @brief Default constructor.
     */
    ShaderProgram() {}

    /**
     * @brief Constructs a ShaderProgram using precompiled shaders.
     * @param vertex The compiled vertex shader.
     * @param fragment The compiled fragment shader.
     */
    ShaderProgram(const Shader &vertex, const Shader &fragment);

    /**
     * @brief Constructs a ShaderProgram from shader source files.
     * @param vertexPath The file path to the vertex shader source.
     * @param fragmentPath The file path to the fragment shader source.
     */
    ShaderProgram(const std::string &vertexPath, const std::string &fragmentPath);

    /**
     * @brief Destructor. Cleans up OpenGL shader program resources.
     */
    virtual ~ShaderProgram();

    /**
     * @brief Activates the shader program for rendering.
     */
    void use() const;

    /**
     * @brief Sets an integer uniform variable in the shader.
     * @param name The name of the uniform variable.
     * @param value The integer value to set.
     */
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(glProgram, name.data()), value);
    }

    /**
     * @brief Sets a float uniform variable in the shader.
     * @param name The name of the uniform variable.
     * @param value The float value to set.
     */
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(glProgram, name.data()), value);
    }

    /**
     * @brief Sets a vec3 uniform variable in the shader.
     * @param name The name of the uniform variable.
     * @param value The glm::vec3 value to set.
     */
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(glProgram, name.data()), 1, &value[0]);
    }

    /**
     * @brief Sets a vec4 uniform variable in the shader.
     * @param name The name of the uniform variable.
     * @param value The glm::vec4 value to set.
     */
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(glProgram, name.data()), 1, &value[0]);
    }

    /**
     * @brief Sets a mat3 uniform variable in the shader.
     * @param name The name of the uniform variable.
     * @param mat The glm::mat3 value to set.
     */
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(glProgram, name.data()), 1, GL_FALSE, &mat[0][0]);
    }

    /**
     * @brief Sets a mat4 uniform variable in the shader.
     * @param name The name of the uniform variable.
     * @param mat The glm::mat4 value to set.
     */
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(glProgram, name.data()), 1, GL_FALSE, &mat[0][0]);
    }

    /**
     * @brief Sets model, view, and projection matrices in the shader.
     * @param model The model transformation matrix.
     * @param view The view transformation matrix.
     * @param projection The projection transformation matrix.
     */
    void setMVPMatrices(const glm::mat4 &model, const glm::mat4 &view, const glm::mat4 &projection);

protected:
    GLuint glProgram; ///< OpenGL shader program ID.
};

#endif // MESH_VIEWER_SHADER_H
