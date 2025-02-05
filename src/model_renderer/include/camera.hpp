//
// Created by pisaarca on 2025/1/14.
//

#ifndef MESH_VIEWER_CAMERA_H
#define MESH_VIEWER_CAMERA_H

#include <glm/glm.hpp>

#include "mesh.hpp"

class Camera
{
public:
    Camera() {}

    explicit Camera(glm::vec3 position, float fov = 90.0f);

    virtual ~Camera() = default;

    [[nodiscard]] glm::mat4 getViewMatrix() const;

    [[nodiscard]] float getFOV() const
    {
        return fov;
    }

    [[nodiscard]] glm::vec3 getPosition() const
    {
        return position;
    }

    void setPosition(const glm::vec3 &newPosition);

    void setFov(float newFov);

    virtual void handleMouseInput(float x, float y, bool pressed);

    virtual void handleKeyboardInput(int key, float deltaTime);

    virtual void handleScrollInput(float y);

    void updateViewMatrix();

    void resetCamera();

    virtual void positionBasedOnObject(const Model &model);

    virtual void setCenter(const glm::vec3 &newCenter) {

    };

protected:
    glm::vec3 position,
        front = {0.0f, 0.0f, -1.0f},
        up = {0.0f, 1.0f, 0.0f},
        right = {1.0f, 0.0f, 0.0f};
    float yaw = -90.0f, pitch = 0.0f, fov;

    bool firstMouseInput = true;
    float lastMouse[2];

    glm::mat4 viewMatrix;
};

class ModelRotationCamera : public Camera
{
public:
    ModelRotationCamera() = default;

    explicit ModelRotationCamera(glm::vec3 center = {0.0f, 0.0f, 0.0f},
                                 float distance = 10.0f) : Camera(center - glm::vec3(0.0f, 0.0f, -distance), 60.0f),
                                                           center(center), distance(distance) {}

    ~ModelRotationCamera() override = default;

    void handleMouseInput(float x, float y, bool pressed) override;

    void handleKeyboardInput(int key, float deltaTime) override;

    void handleScrollInput(float y) override;

    void positionBasedOnObject(const Model &model) override;

    void setCenter(const glm::vec3 &newCenter) override;

protected:
    glm::vec3 center;
    float distance;
};

#endif // MESH_VIEWER_CAMERA_H
