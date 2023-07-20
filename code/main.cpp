#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <cmath>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui.h"
#include "backends/imgui_impl_opengl3.h"
#include "backends/imgui_impl_glfw.h"

#include "stb_image.h"
#include "shader.hpp"
#include "camera.hpp"
#include "mesh.hpp"
#include "topo_mesh_3D.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>


/* LINUX
mkdir build
cd build
cmake ..
make
*/

// camera
glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f,  4.5f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.f;
float lastFrame = 0.f;

float lastX = 400, lastY = 300;
bool firstMouse = true;

bool wireframe = false;

Camera camera = Camera(cameraPos, cameraUp, -90.f, 0.f);
Orbiter orbiter = Orbiter(cameraPos);

void processInput(GLFWwindow *window);
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);


int main(int, char**) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    int size_x = 1200;
    int size_y = 800;
    GLFWwindow* window = glfwCreateWindow(size_x, size_y, "LearnOpenGL", NULL, NULL);
    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    //glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    glViewport(0, 0, size_x, size_y);

    glewInit();


    // IMGUI
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330");



    std::cout << "Hello, world!\n";

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_BACK);

    Shader object_shader = Shader("data/shaders/object.vert.glsl", "data/shaders/object.frag.glsl");
    Shader debug_shader = Shader("data/shaders/debug.vert.glsl", "data/shaders/debug.frag.glsl");

    Mesh mesh_frame = Mesh::Frame();
    Mesh mesh_grid = Mesh::Grid(4);
    //Mesh mesh = Mesh("../ressources/splash/splash.gltf");
    //Mesh mesh = Mesh("../ressources/samples/centurion_helmet.obj");
    //Mesh mesh = Mesh("../ressources/samples/statue.stl");
    //Mesh mesh_1 = Mesh("data/meshs/centurion_helmet.obj");
    Mesh mesh_1 = Mesh("data/meshs/bunny.obj");
    TopoMesh3D topo_mesh = TopoMesh3D(mesh_1);

    std::vector<unsigned int> valences = topo_mesh.GetValence();
    for (int i = 0; i < valences.size(); i++) std::cout << i << " : " << valences[i] << std::endl;

    Mesh mesh = topo_mesh.ToGlMesh();
    //Mesh mesh = Mesh::Sphere(10);
    //mesh.setupMesh();
    //Mesh mesh = Mesh("../ressources/minifig/lego.obj");

    
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, glm::radians(0.0f), glm::vec3(1.0f, -1.0f, 0.0f));

    while(!glfwWindowShouldClose(window)) {
        processInput(window);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        //ImGui::ShowDemoWindow();
        ImGui::SetNextWindowPos(ImVec2(10, 10));
        ImGui::SetNextWindowSize(ImVec2(180, 50));
        ImGui::Begin("Infos");
        ImGui::Text("%.3f ms  -  %.1f FPS", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();

        ImGui::SetNextWindowPos(ImVec2(10, 80));
        ImGui::SetNextWindowSize(ImVec2(180, 60));
        ImGui::Begin("Options");
        ImGui::Checkbox(" WireFrame", &wireframe);
        ImGui::End();
        

        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        object_shader.use();

        // MVP
        glm::mat4 view = orbiter.GetViewMatrix();
        glm::mat4 projection = glm::perspective(glm::radians(orbiter.Zoom), float(size_x) / float(size_y), 0.1f, 100.0f);
        object_shader.setMat4("model", model);
        object_shader.setMat4("view", view);
        object_shader.setMat4("projection", projection);

        object_shader.setVec3("lightPos", glm::vec3(0, 0, 3));
        object_shader.setVec3("cameraPos", orbiter.Position);
        object_shader.setVec3("min_vertex", mesh.min_vertex);
        object_shader.setVec3("max_vertex", mesh.max_vertex);

        if(wireframe) glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        mesh.Draw(object_shader);

        debug_shader.use();
        debug_shader.setMat4("model", model);
        debug_shader.setMat4("view", view);
        debug_shader.setMat4("projection", projection);
        mesh_frame.Draw(debug_shader);
        mesh_grid.Draw(debug_shader);

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        
        glfwSwapBuffers(window);
        glfwPollEvents();   
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glDeleteProgram(object_shader.ID);
    glDeleteProgram(debug_shader.ID);
    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow *window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; 
    lastX = xpos;
    lastY = ypos;
    if(glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT)) {
        camera.ProcessLeftMouseMovement(xoffset, yoffset);
        orbiter.ProcessLeftMouseMovement(xoffset, yoffset);
    } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT)) {
        //orbiter.ProcessRightMouseMovement(xoffset, yoffset);
    }
} 

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    camera.ProcessMouseScroll(yoffset);
    orbiter.ProcessMouseScroll(yoffset);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}