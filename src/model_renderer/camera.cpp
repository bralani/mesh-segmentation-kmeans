//
// Created by pisaarca on 2025/1/14.
//

#include "camera.hpp"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.hpp"
#include <iostream>

static constexpr float cameraMouseSpeed = 0.05f,
                       cameraKeyboardSpeed = 10.0f,
                       cameraScrollSpeed = 5.0f;

Camera::Camera(glm::vec3 position, float fov) : position(position), fov(fov) {}

glm::mat4 Camera::getViewMatrix() const
{
    return glm::lookAt(position, position + front, up);
}

void Camera::handleMouseInput(float x, float y, bool pressed)
{
    if (firstMouseInput || !pressed)
    {
        lastMouse[0] = x;
        lastMouse[1] = y;
        firstMouseInput = false;
        return;
    }

    float dx = x - lastMouse[0], dy = y - lastMouse[1];
    yaw -= dx * cameraMouseSpeed;
    pitch += dy * cameraMouseSpeed;
    pitch = std::fmax(std::fmin(pitch, 90.0f), -90.0f); // clamp to [-90.0, 90.0] degree

    front = glm::normalize(glm::vec3(
        std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
        std::sin(glm::radians(pitch)),
        std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))));
    right = glm::cross(front, up);

    lastMouse[0] = x;
    lastMouse[1] = y;
}

void Camera::handleKeyboardInput(int key, float deltaTime)
{
    if (key == GLFW_KEY_S)
    {
        position += up * cameraKeyboardSpeed * deltaTime; // Move right
    }
    else if (key == GLFW_KEY_W)
    {
        position -= up * cameraKeyboardSpeed * deltaTime; // Move left
    }
    else if (key == GLFW_KEY_D)
    {
        position -= right * cameraKeyboardSpeed * deltaTime; // Move up
    }
    else if (key == GLFW_KEY_A)
    {
        position += right * cameraKeyboardSpeed * deltaTime; // Move down
    }
    else if (key == GLFW_KEY_U)
    {
        position += front * cameraKeyboardSpeed * deltaTime; // Move far
    }
    else if (key == GLFW_KEY_J)
    {
        position -= front * cameraKeyboardSpeed * deltaTime; // Move close
    }
}

void Camera::handleScrollInput(float y)
{
    fov -= y * cameraScrollSpeed;

    // Clamp to a more reasonable range to avoid the mesh to became a point in space
    fov = std::fmax(std::fmin(fov, 80.0f), 1.0f);
}

void Camera::updateViewMatrix()
{
    glm::vec3 target = position + front;
    viewMatrix = glm::lookAt(position, target, up);
}

void Camera::setPosition(const glm::vec3 &newPosition)
{
    position = newPosition;
    updateViewMatrix(); // Update the view matrix when the position changes
}

void Camera::setFov(float newFov)
{
    // Ensure the FOV is within a reasonable range
    fov = glm::clamp(newFov, 20.0f, 100.0f);

    std::cout << "Updated FOV: " << fov << std::endl;
}

float calculateCameraDistance(const glm::vec3 &objectSize, float factor = 1.5f)
{
    float diagonal = glm::length(objectSize); // Calculate diagonal using glm::length for the bounding box
    return diagonal * factor;
}

void Camera::positionBasedOnObject(const Model &model)
{
    // Calculate the bounding box size of the object
    glm::vec3 objectSize = model.getBoundingBoxSize();
    glm::vec3 objectCenter = model.getBoundingBoxCenter();

    // Determine a distance to position the camera
    float cameraDistance = std::max(objectSize.x, objectSize.y) * 1.5f;

    // Set the camera's position
    glm::vec3 cameraPosition = objectCenter + glm::vec3(0.0f, 0.0f, cameraDistance); // Adjusting along z-axis

    this->setPosition(cameraPosition); // Assuming the camera has a setPosition function
}

void Camera::resetCamera()
{
    fov = 20.0f;

    position = {0.0f, 10.0f, 0.0f};
    front = {0.0f, 0.0f, -1.0f};
    up = {0.0f, 1.0f, 0.0f};
    right = {1.0f, 0.0f, 0.0f};

    yaw = -90.0f;
    pitch = 0.0f;
}

static float modelRotationCameraMouseSpeed = 0.1f,
             modelRotationCameraScrollSpeed = 1.0f;

void ModelRotationCamera::handleMouseInput(float x, float y, bool pressed)
{
    if (firstMouseInput || !pressed)
    {
        lastMouse[0] = x;
        lastMouse[1] = y;
        firstMouseInput = false;
        return;
    }

    float dx = x - lastMouse[0], dy = y - lastMouse[1];
    yaw += dx * modelRotationCameraMouseSpeed;
    pitch -= dy * modelRotationCameraMouseSpeed;

    // Clamp pitch to avoid flipping
    pitch = std::fmax(std::fmin(pitch, 89.9f), -89.9f);

    front = glm::normalize(glm::vec3(
        std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
        std::sin(glm::radians(pitch)),
        std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))));

    // Update position based on center and distance
    position = center - front * distance;

    lastMouse[0] = x;
    lastMouse[1] = y;
}

void ModelRotationCamera::handleKeyboardInput(int key, float deltaTime)
{
    Camera::handleKeyboardInput(key, deltaTime);
    center = position + front * distance;
}

void ModelRotationCamera::positionBasedOnObject(const Model &model)
{
    glm::vec3 objectSize = model.getBoundingBoxSize();
    distance = calculateCameraDistance(objectSize);

    std::cout << "Distance calculated: " << distance << std::endl;
}

void ModelRotationCamera::handleScrollInput(float y)
{
    distance -= y * modelRotationCameraScrollSpeed;
    // distance = std::fmax(std::fmin(distance, 100.0f), 0.1f); // Clamp distance to avoid negative values
    position = center - front * distance;
}

void ModelRotationCamera::setCenter(const glm::vec3 &newCenter)
{
    center = newCenter;
    position = center - front * distance;
}