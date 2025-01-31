//
// Created by pisaarca on 2025/1/10.
//

#ifndef RENDER_HPP
#define RENDER_HPP

#include <functional>
#include <string>

class Render
{
public:
    // Constructor accepting a reference-based callback function
    explicit Render(std::function<void(Render &, const std::string &)> segmentationCallback);

    // Starts the rendering loop
    void start();

    // Render a specific file
    void renderFile(const std::string &fileName);

private:
    std::function<void(Render &, const std::string &)> segmentationCallback;
};

#endif // RENDER_HPP