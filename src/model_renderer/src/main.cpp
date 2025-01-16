#include <iostream>

#ifdef _WIN32
#include <windows.h>
#endif
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <filesystem>
#include <iostream>

#include <plog/Log.h>
#include <plog/Initializers/RollingFileInitializer.h>

#include "shader.h"
#include "mesh.h"
#include "camera.h"
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

int width = 1600, height = 900;
float lastTime;
Camera *camera = nullptr;

// List of available models
std::vector<std::string> modelPaths;

void framebuffer_size_callback(GLFWwindow *window, int width, int height);
void cursor_position_callback(GLFWwindow *window, double x, double y);
void scroll_callback(GLFWwindow *window, double x, double y);
void handle_keyboard(GLFWwindow *window, float deltaTime);

// Function to populate modelPaths
void populateModelPaths(const std::string &directory) {
    try {
        // Iterate through the directory
        for (const auto &entry : std::filesystem::directory_iterator(directory)) {
            if (entry.is_regular_file() && entry.path().extension() == ".obj") {
                modelPaths.push_back(entry.path().string());  // Add the .obj file to the list
            }
        }
    } catch (const std::filesystem::filesystem_error &e) {
        std::cerr << "Error reading directory: " << e.what() << std::endl;
    }
}

int main() {
    // Directory to search for .obj files
    std::string modelsDirectory = "../resource/lumine/";

    // Populate the modelPaths vector
    populateModelPaths(modelsDirectory);

    // Debug: Print loaded model paths
    for (const auto &model : modelPaths) {
        std::cout << "Loaded model: " << model << std::endl;
    }

    plog::init(plog::warning);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);

    // Create window
    GLFWwindow *window = glfwCreateWindow(width, height, "Mesh Viewer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Init glad
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, cursor_position_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // ImGui Initialization
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");

    ShaderProgram program("../src/shader/common.vert", "../src/shader/phong.frag");

    // Variables for model selection
    int selectedModelIndex = -1;  // Default: no model selected
    std::string selectedModelPath;
    Model *currentModel = nullptr;  // Pointer to the currently loaded model
    bool renderModel = false;  // Flag to trigger rendering after selection

    camera = new ModelRotationCamera({0.0f, 10.0f, 0.0f}, 20.0f);

    lastTime = glfwGetTime();

    glm::vec3 lightPos(0.0f, 10.0f, 5.0f),
            lightColor(3.0f, 3.0f, 1.0f),
            lightAmbient(1.0f, 1.0f, 1.0f);

    glEnable(GL_DEPTH_TEST);

    while (!glfwWindowShouldClose(window)) {
        // Clear the screen
        glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ImGui Frame Initialization
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // ImGui Menu
        ImGui::Begin("Model Selector");

        ImGui::Text("Select a Model to Load:");
        for (int i = 0; i < modelPaths.size(); ++i) {
            if (ImGui::Selectable(modelPaths[i].c_str(), selectedModelIndex == i)) {
                selectedModelIndex = i;
                selectedModelPath = modelPaths[i];  // Store the selected path
            }
        }

        // Render Button
        if (ImGui::Button("Render") && selectedModelIndex != -1) {
            // Load the selected model
            if (currentModel) {
                delete currentModel;  // Unload the current model
                currentModel = nullptr;
            }

            currentModel = new Model(selectedModelPath, &program);
            camera->positionBasedOnObject(*currentModel);
            renderModel = true;  // Trigger rendering
        }

        ImGui::End();

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and render the selected model
        if (renderModel && currentModel) {
            float nowTime = glfwGetTime();
            float deltaTime = nowTime - lastTime;
            lastTime = nowTime;
            handle_keyboard(window, deltaTime);

            glm::mat4 model = glm::identity<glm::mat4>();
            glm::mat4 view = camera->getViewMatrix();
            glm::mat4 projection = glm::perspective(glm::radians(camera->getFOV()),
                                                    (float) width / height, 0.1f, 100.0f);

            program.setMVPMatrices(model, view, projection);
            program.setVec3("eyePos", camera->getPosition());
            program.setVec3("light.position", lightPos);
            program.setVec3("light.ambient", lightAmbient);
            program.setVec3("light.diffuse", lightColor);
            program.setVec3("light.specular", lightColor);
            currentModel->draw();
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    if (currentModel) delete currentModel;
    delete camera;

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}

// Callback implementations (unchanged)
void framebuffer_size_callback(GLFWwindow *window, int width, int height) {
    ::width = width;
    ::height = height;
    glViewport(0, 0, width, height);
}

void cursor_position_callback(GLFWwindow *window, double x, double y) {
    bool pressed = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
    if (camera != nullptr) camera->handleMouseInput(x, y, pressed);
}

void scroll_callback(GLFWwindow *window, double x, double y) {
    if (camera != nullptr) camera->handleScrollInput(y);
}

void handle_keyboard(GLFWwindow *window, float deltaTime) {
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (camera != nullptr) camera->handleKeyboardInput(GLFW_KEY_W, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (camera != nullptr) camera->handleKeyboardInput(GLFW_KEY_A, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (camera != nullptr) camera->handleKeyboardInput(GLFW_KEY_S, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (camera != nullptr) camera->handleKeyboardInput(GLFW_KEY_D, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        if (camera != nullptr) camera->handleKeyboardInput(GLFW_KEY_U, deltaTime);
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        if (camera != nullptr) camera->handleKeyboardInput(GLFW_KEY_J, deltaTime);
    }
}
