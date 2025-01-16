//
// Created by pisaarca on 2025/1/14.
//

#include "camera.h"

#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include "mesh.h"

static constexpr float cameraMouseSpeed = 0.05f,
        cameraKeyboardSpeed = 10.0f,
        cameraScrollSpeed = 5.0f;

Camera::Camera(glm::vec3 position, float fov) :
        position(position), fov(fov) {}

glm::mat4 Camera::getViewMatrix() const {
    return glm::lookAt(position, position + front, up);
}

void Camera::handleMouseInput(float x, float y, bool pressed) {
    if (firstMouseInput || !pressed) {
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
            std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))
    ));
    right = glm::cross(front, up);

    lastMouse[0] = x;
    lastMouse[1] = y;
}

void Camera::handleKeyboardInput(int key, float deltaTime) {
    if (key == GLFW_KEY_S) {
        position += up * cameraKeyboardSpeed * deltaTime;
    } else if (key == GLFW_KEY_W) {
        position -= up * cameraKeyboardSpeed * deltaTime;
    } else if (key == GLFW_KEY_D) {
        position -= right * cameraKeyboardSpeed * deltaTime;
    } else if (key == GLFW_KEY_A) {
        position += right * cameraKeyboardSpeed * deltaTime;
    } else if (key == GLFW_KEY_U) {
        position += front * cameraKeyboardSpeed * deltaTime;  // Move up
    } else if (key == GLFW_KEY_J) {
        position -= front * cameraKeyboardSpeed * deltaTime;  // Move down
    }
}

void Camera::handleScrollInput(float y) {
    fov -= y * cameraScrollSpeed;
    fov = std::fmax(std::fmin(fov, 80.0f), 1.0f);
}

void Camera::updateViewMatrix() {
    // Assuming you have a lookAt function to update the view matrix
    viewMatrix = glm::lookAt(position, position, up);
}

void Camera::setPosition(const glm::vec3& newPosition) {
    position = newPosition;
    updateViewMatrix();  // Update the view matrix when the position changes
}

void Camera::positionBasedOnObject(const Model& model) {
    // Calculate the bounding box size of the object
    glm::vec3 objectSize = model.getBoundingBoxSize();
    glm::vec3 objectCenter = glm::vec3(0.0f);  // Assuming the object is centered at the origin

    // Determine a distance to position the camera
    float distance = std::max(objectSize.x, objectSize.y) * 1.5f;

    // Set the camera's position
    glm::vec3 cameraPosition = objectCenter + glm::vec3(0.0f, 0.0f, distance);  // Adjusting along z-axis

    this->setPosition(cameraPosition);  // Assuming the camera has a setPosition function
}

static float modelRotationCameraMouseSpeed = 0.1f,
        modelRotationCameraScrollSpeed = 1.0f;

void ModelRotationCamera::handleMouseInput(float x, float y, bool pressed) {
    if (firstMouseInput || !pressed) {
        lastMouse[0] = x;
        lastMouse[1] = y;
        firstMouseInput = false;
        return;
    }

    float dx = x - lastMouse[0], dy = y - lastMouse[1];
    yaw += dx * modelRotationCameraMouseSpeed;
    pitch -= dy * modelRotationCameraMouseSpeed;
    pitch = std::fmax(std::fmin(pitch, 90.0f), -90.0f); // clamp to [-90.0, 90.0] degree

    front = glm::normalize(glm::vec3(
            std::cos(glm::radians(yaw)) * std::cos(glm::radians(pitch)),
            std::sin(glm::radians(pitch)),
            std::sin(glm::radians(yaw)) * std::cos(glm::radians(pitch))
    ));
    right = glm::cross(front, up);
    position = center - front * distance;

    lastMouse[0] = x;
    lastMouse[1] = y;
}

void ModelRotationCamera::handleKeyboardInput(int key, float deltaTime) {
    Camera::handleKeyboardInput(key, deltaTime);
    center = position + front * distance;
}

void ModelRotationCamera::handleScrollInput(float y) {
    distance -= y * modelRotationCameraScrollSpeed;
    distance = std::fmax(std::fmin(distance, 100.0f), 1e-6f);
    position = center - front * distance;
}
