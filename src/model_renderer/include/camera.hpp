//
// Created by pisaarca on 2025/1/14.
//

#ifndef MESH_VIEWER_CAMERA_H
#define MESH_VIEWER_CAMERA_H

#include <glm/glm.hpp>
#include "mesh.hpp"

/**
 * @class Camera
 * @brief Base class for a camera in a 3D scene.
 *
 * Provides functionalities for handling camera movement, input handling, and
 * view matrix calculations.
 */
class Camera
{
public:
    /**
     * @brief Default constructor.
     */
    Camera() {}

    /**
     * @brief Constructs a Camera with a given position and field of view.
     * @param position The initial position of the camera.
     * @param fov The field of view in degrees (default is 90.0f).
     */
    explicit Camera(glm::vec3 position, float fov = 90.0f);

    /**
     * @brief Virtual destructor.
     */
    virtual ~Camera() = default;

    /**
     * @brief Gets the view matrix for rendering.
     * @return The view matrix (glm::mat4).
     */
    [[nodiscard]] glm::mat4 getViewMatrix() const;

    /**
     * @brief Gets the field of view (FOV).
     * @return The FOV in degrees.
     */
    [[nodiscard]] float getFOV() const
    {
        return fov;
    }

    /**
     * @brief Gets the current position of the camera.
     * @return The camera position as a glm::vec3.
     */
    [[nodiscard]] glm::vec3 getPosition() const
    {
        return position;
    }

    /**
     * @brief Sets a new camera position.
     * @param newPosition The new position for the camera.
     */
    void setPosition(const glm::vec3 &newPosition);

    /**
     * @brief Sets a new field of view (FOV).
     * @param newFov The new FOV in degrees.
     */
    void setFov(float newFov);

    /**
     * @brief Handles mouse input for camera movement.
     * @param x The x-coordinate of the mouse.
     * @param y The y-coordinate of the mouse.
     * @param pressed True if the mouse button is pressed.
     */
    virtual void handleMouseInput(float x, float y, bool pressed);

    /**
     * @brief Handles keyboard input for camera movement.
     * @param key The key pressed (GLFW key values).
     * @param deltaTime The time between frames, used for smooth movement.
     */
    virtual void handleKeyboardInput(int key, float deltaTime);

    /**
     * @brief Handles scroll input for zooming or changing FOV.
     * @param y The scroll offset value.
     */
    virtual void handleScrollInput(float y);

    /**
     * @brief Updates the view matrix based on camera transformations.
     */
    void updateViewMatrix();

    /**
     * @brief Resets the camera to its default position and orientation.
     */
    void resetCamera();

    /**
     * @brief Positions the camera based on the given model.
     * @param model The 3D model to align the camera with.
     */
    virtual void positionBasedOnObject(const Model &model);

    /**
     * @brief Sets the center of rotation for the camera.
     * @param newCenter The new center position.
     */
    virtual void setCenter(const glm::vec3 &newCenter) {}

protected:
    glm::vec3 position;                    ///< The position of the camera.
    glm::vec3 front = {0.0f, 0.0f, -1.0f}; ///< The front direction of the camera.
    glm::vec3 up = {0.0f, 1.0f, 0.0f};     ///< The up direction of the camera.
    glm::vec3 right = {1.0f, 0.0f, 0.0f};  ///< The right direction of the camera.

    float yaw = -90.0f; ///< The yaw angle (rotation around the Y-axis).
    float pitch = 0.0f; ///< The pitch angle (rotation around the X-axis).
    float fov;          ///< The field of view (FOV) in degrees.

    bool firstMouseInput = true; ///< Flag to track the first mouse input.
    float lastMouse[2];          ///< Stores the last mouse position (x, y).

    glm::mat4 viewMatrix; ///< The view matrix representing the camera transformation.
};

/**
 * @class ModelRotationCamera
 * @brief A specialized camera that rotates around a 3D model.
 *
 * This camera orbits around a central point, allowing for intuitive viewing
 * of a 3D object from various angles.
 */
class ModelRotationCamera : public Camera
{
public:
    /**
     * @brief Default constructor.
     */
    ModelRotationCamera() = default;

    /**
     * @brief Constructs a ModelRotationCamera with a center and distance.
     * @param center The center point to orbit around.
     * @param distance The distance from the center.
     */
    explicit ModelRotationCamera(glm::vec3 center = {0.0f, 0.0f, 0.0f},
                                 float distance = 10.0f)
        : Camera(center - glm::vec3(0.0f, 0.0f, -distance), 60.0f),
          center(center), distance(distance) {}

    /**
     * @brief Destructor.
     */
    ~ModelRotationCamera() override = default;

    /**
     * @brief Handles mouse input for rotating the camera around the model.
     * @param x The x-coordinate of the mouse.
     * @param y The y-coordinate of the mouse.
     * @param pressed True if the mouse button is pressed.
     */
    void handleMouseInput(float x, float y, bool pressed) override;

    /**
     * @brief Handles keyboard input for modifying the camera’s position.
     * @param key The key pressed.
     * @param deltaTime The time difference between frames.
     */
    void handleKeyboardInput(int key, float deltaTime) override;

    /**
     * @brief Handles scroll input to zoom in and out around the model.
     * @param y The scroll offset.
     */
    void handleScrollInput(float y) override;

    /**
     * @brief Positions the camera based on a given model.
     * @param model The model to align the camera with.
     */
    void positionBasedOnObject(const Model &model) override;

    /**
     * @brief Sets a new center point for the camera's orbit.
     * @param newCenter The new center point.
     */
    void setCenter(const glm::vec3 &newCenter) override;

protected:
    glm::vec3 center; ///< The center point for orbiting the camera.
    float distance;   ///< The distance from the center.
};

#endif // MESH_VIEWER_CAMERA_H
