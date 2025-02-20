//
// Created by pisaarca on 2025/1/10.
//

#ifndef RENDER_HPP
#define RENDER_HPP

#include "shader.hpp"
#include <functional>
#include <string>
#include "SharedEnum.hpp"

/**
 * @class Render
 * @brief Handles rendering and segmentation callbacks for a 3D mesh viewer.
 *
 * This class manages the rendering process, model loading, and segmentation callbacks.
 * It provides functions to start the rendering loop, load models, and trigger rendering for specific files.
 */
class Render
{
public:
    /**
     * @brief Constructs a Render instance with a segmentation callback.
     * @param segmentationCallback A function that performs segmentation and returns the output file name.
     *
     * The callback should take the model file path, initialization methods,
     * metric method, number of clusters, and threshold as input.
     */
    explicit Render(std::function<std::string(const std::string &, Enums::CentroidInit,
                                              Enums::KInit, Enums::MetricMethod, int, double)>
                        segmentationCallback);

    /**
     * @brief Starts the rendering loop.
     *
     * This function initializes the rendering environment, sets up OpenGL context,
     * and enters the main rendering loop where the user interacts with the UI.
     */
    void start();

    /**
     * @brief Loads a model from a file and prepares it for rendering.
     * @param modelPath The path to the model file.
     *
     * This function loads the model, sets up the necessary OpenGL buffers,
     * and updates the rendering pipeline.
     */
    void loadAndRenderModel(const std::string &modelPath);

    /**
     * @brief Renders a specific file.
     * @param fileName The path to the file to be rendered.
     *
     * This function is used to display a segmented model after processing.
     */
    void renderFile(const std::string &fileName);

private:
    /**
     * @brief Callback function for segmentation.
     *
     * This function processes a mesh segmentation task and returns the
     * output file path of the segmented model.
     */
    std::function<std::string(const std::string &, Enums::CentroidInit,
                              Enums::KInit, Enums::MetricMethod, int, double)>
        segmentationCallback;

    ShaderProgram *program; ///< Pointer to the shader program used for rendering.
};

#endif // RENDER_HPP
