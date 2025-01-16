//
// Created by pisaarca on 2025/1/10.
//

#include "mesh.h"

#include <OBJ_Loader.h>

Mesh::Mesh(const objl::Mesh &mesh, Material *material) :
        indexSize(mesh.Indices.size()), material(material) {
    vertices = new float[8 * mesh.Vertices.size()];
    indices = new unsigned int[mesh.Indices.size()];
    memcpy(vertices, mesh.Vertices.data(), 8 * sizeof(float) * mesh.Vertices.size());
    memcpy(indices, mesh.Indices.data(), sizeof(unsigned int) * mesh.Indices.size());

    glGenVertexArrays(1, &glVAO);
    glGenBuffers(1, &glVBO);
    glGenBuffers(1, &glEBO);

    glBindVertexArray(glVAO);
    glBindBuffer(GL_ARRAY_BUFFER, glVBO);
    glBufferData(GL_ARRAY_BUFFER, 8 * sizeof(float) * mesh.Vertices.size(), vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, glEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * mesh.Indices.size(), indices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) 0); // position
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (3 * sizeof(float))); // normal
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *) (6 * sizeof(float))); // texcoord

    glBindVertexArray(0);
}

Mesh::~Mesh() {
    glDeleteBuffers(1, &glVBO);
    glDeleteBuffers(1, &glEBO);
    glDeleteVertexArrays(1, &glVAO);

    delete[] vertices;
    delete[] indices;
}

void Mesh::draw() const {
    if (material == nullptr) {
        std::cerr << "Error: Material is nullptr!" << std::endl;
        return;
    }
    material->use();
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

    glBindVertexArray(glVAO);
    glDrawElements(GL_TRIANGLES, indexSize, GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

Model::Model(const std::string &path, ShaderProgram *program) : program(program) {
    if (!std::filesystem::exists(path)) {
        PLOG_FATAL << "Model Load Error: file not found " + path;
        throw std::runtime_error("File not found: " + path);
    }
    loader = new objl::Loader;
    if (!loader->LoadFile(path)) {
        PLOG_FATAL << "Model Load Error: unable to load model " + path;
        throw std::runtime_error("Unable to load model: " + path);
    }

    auto objDir = std::filesystem::path(path).parent_path().string();

    unsigned char whitePixel[3] = {255, 255, 255};
    defaultTexture = new Texture2D(whitePixel, 1, 1);
    auto defaultMaterial = new PhongMaterial(program, {defaultTexture}, 0, 0, 32.0f); // Default white material
    materials["default"] = defaultMaterial;

    for (const auto &_material : loader->LoadedMaterials) {
        std::cout << "Material Name: " << _material.name << "\n";
        std::cout << "map_Kd (Texture Path): " << _material.map_Kd << "\n";
        std::cout << "Shininess (Ns): " << _material.Ns << "\n";

        Texture2D *texture = nullptr;
        if (!_material.map_Kd.empty()) {
            auto texturePath = objDir + "/" + _material.map_Kd;
            texture = loadTexture(texturePath);
        }

        if (!texture) {
            std::cerr << "Warning: Missing texture for material '" << _material.name
                      << "'. Using default white texture.\n";
            texture = defaultTexture; // Use white texture as fallback
        }

        auto material = new PhongMaterial(program, {texture}, 0, 0, _material.Ns);
        materials[_material.name] = material;

        std::cout << "Texture Loaded " << "\n";
        std::cout << "Material Created " << "\n";
        std::cout << "----------------------------------------\n";
    }

    for (const auto &_mesh : loader->LoadedMeshes) {
        Material *meshMaterial = nullptr;

        if (materials.contains(_mesh.MeshMaterial.name)) {
            meshMaterial = materials[_mesh.MeshMaterial.name];
        } else {
            std::cerr << "Warning: Missing material for mesh. Falling back to default material.\n";
            meshMaterial = defaultMaterial;
        }

        meshes.push_back(new Mesh(_mesh, meshMaterial));
    }
}

Model::~Model() {
    delete loader;
    for (const auto mesh: meshes) {
        delete mesh;
    }
    for (const auto &material: materials) {
        delete material.second;
    }
    for (const auto &texture: textures) {
        delete texture.second;
    }

    delete defaultTexture;
}

void Model::draw() const {
    for (const auto &mesh : meshes) {
        if (mesh != nullptr) {
            mesh->draw();
        } else {
            std::cerr << "Found a nullptr in meshes" << std::endl;
        }
    }
}

// Assuming you have a function like this in your Model class
glm::vec3 Model::getBoundingBoxSize() const {
    glm::vec3 minBounds = glm::vec3(INFINITY);
    glm::vec3 maxBounds = glm::vec3(-INFINITY);

    for (const auto& mesh : meshes) {
        for (size_t i = 0; i < mesh->getVertexCount(); i++) {
            // Assuming the getVertex function returns a float pointer to the vertex data
            float* vertexData = mesh->getVertex(i);
            if (vertexData) {
                // Construct the glm::vec3 from the vertex data (x, y, z)
                glm::vec3 vertex(vertexData[0], vertexData[1], vertexData[2]);
                // Now you can use the vertex as a glm::vec3
                minBounds = glm::min(minBounds, vertex);
                maxBounds = glm::max(maxBounds, vertex);
            }
        }
    }

    return maxBounds - minBounds;
}

Texture2D *Model::loadTexture(const std::string &path) {
    if (textures.contains(path)) return textures[path];
    return textures[path] = new Texture2D(path);
}
