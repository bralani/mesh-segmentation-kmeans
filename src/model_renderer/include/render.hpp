//
// Created by pisaarca on 2025/1/10.
//

#ifndef RENDER_HPP
#define RENDER_HPP

#include "shader.hpp"
#include <functional>
#include <string>
#include "SharedEnum.hpp"

class Render
{
public:
    // Constructor accepting a reference-based callback function
    explicit Render(std::function<void(Render &, const std::string &, Enums::CentroidInit, Enums::KInit, int, double)> segmentationCallback);

    // Starts the rendering loop
    void start();

    void loadAndRenderModel(const std::string &modelPath);

    // Render a specific file
    void renderFile(const std::string &fileName);

private:
    std::function<void(Render &, const std::string &, Enums::CentroidInit, Enums::KInit, int, double)> segmentationCallback;

    ShaderProgram *program;
};

#endif // RENDER_HPP