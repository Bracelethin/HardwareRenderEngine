#include <glad/glad.h>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#ifndef SHADER
    #define SHADER "./"
#endif
#ifndef RESOURCES
    #define RESOURCES "./"
#endif
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION

#include <string>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "renderPass.h"
#include "model.h"
#include "Light.h"
#include <iostream>

const std::string shaderPath(SHADER);
const std::string resourcesPath(RESOURCES);


void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window, int keycode, int scancode, int action, int mods);
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
PointLight light;
bool leftCursordown=false;


// timing
float deltaTime = 0.0f;	// time between current frame and last frame
float lastFrame = 0.0f;
static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}
void imguiWindows(){

//            static float f = 0.0f;
//            static int counter = 0;

            ImGui::Begin("Hello, world!");                          // Create a window called "Hello, world!" and append into it.

//            ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
//
//            ImGui::SliderFloat("float", &f, 0.0f, 1.0f);            // Edit 1 float using a slider from 0.0f to 1.0f
//
//            if (ImGui::Button("Button"))                            // Buttons return true when clicked (most widgets return true when edited/activated)
//                counter++;
//            ImGui::SameLine();
//            ImGui::Text("counter = %d", counter);

            ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("camera Position (%f,%f,%f)",camera.Position.x,camera.Position.y, camera.Position.z);
            ImGui::End();
}
GLFWwindow* init(){

    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        exit(-1);
    
    glfwInit();
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        exit(-1);
    }
    glfwMakeContextCurrent(window);


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    //io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init(glsl_version);


    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, processInput);
    glfwSetMouseButtonCallback(window, MouseButtonCallback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        exit(-1);
    }

    glPointSize(1);
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    return window;
}


void setUniform(Shader& s, Shader& ls){
    s.use();
    glm::vec3 lp=light.location;
    s.setVec3("light.position",lp.x,lp.y,lp.z);
    s.setVec3("light.ambient",0.00,0.00,0.00);
    s.setVec3("light.diffuse",0.5,0.5,0.5);
    s.setVec3("light.specular",1,1,1);
    s.setVec3("viewPos",camera.Position.x,camera.Position.y,camera.Position.z);

    glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
    glm::mat4 view = camera.GetViewMatrix();
    s.setMat4("projection", projection);
    s.setMat4("view", view);

    glm::mat4 model = glm::mat4(1.0f);
    s.setMat4("model", model);
    model=glm::scale(model,glm::vec3(0.1));
    model=glm::translate(model, lp);
    ls.use();
    ls.setMat4("projection", projection);
    ls.setMat4("view", view);
    ls.setMat4("model", model);
}

void renderPass()
{
    GLFWwindow* window = init();
    Shader casterShader((shaderPath+"/shader/phong/vs.glsl").c_str(), (shaderPath+"/shader/phong/fs.glsl").c_str());
    Shader lightShader((shaderPath+"/shader/phong/vsLight.glsl").c_str(), (shaderPath+"/shader/phong/fsLight.glsl").c_str());
    Model m1(resourcesPath+"/backpack/backpack.obj");
    light=PointLight(glm::vec3(0,0,20));
    printf("=========start loop=========\n");
    while (!glfwWindowShouldClose(window))
    {
        
        glfwPollEvents();
        
        
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        
        imguiWindows();
        ImGui::Render();
        
        // per-frame time logic
        // --------------------
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        setUniform(casterShader, lightShader);
        casterShader.use();
        m1.Draw(casterShader);
        lightShader.use();
        light.Draw(lightShader);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        glfwSwapBuffers(window);
    }
    glfwTerminate();
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// --------------------------------------------------------------------------------------------------------

void processInput(GLFWwindow* window, int keycode, int scancode, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    ImGuiKey imgui_key = ImGui_ImplGlfw_KeyToImGuiKey(keycode);
    io.AddKeyEvent(imgui_key, (action == GLFW_PRESS));
    if (!io.WantCaptureKeyboard){
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
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMousePosEvent((float)xposIn, (float)yposIn);
    if(!io.WantCaptureMouse){
        if(leftCursordown){
            float xpos = static_cast<float>(xposIn);
            float ypos = static_cast<float>(yposIn);

            if (firstMouse)
            {
                lastX = xpos;
                lastY = ypos;
                firstMouse = false;
            }

            float xoffset = xpos - lastX;
            float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

            lastX = xpos;
            lastY = ypos;

            camera.ProcessMouseMovement(xoffset, yoffset);
        }
    }
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    ImGuiIO& io = ImGui::GetIO();
    io.AddMouseWheelEvent((float)xoffset, (float)yoffset);
    if(!io.WantCaptureMouse){
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
    }
}
void MouseButtonCallback(GLFWwindow* window, int button, int action, int mods)
{
    ImGuiIO& io = ImGui::GetIO();
    if (button >= 0 && button < ImGuiMouseButton_COUNT)
        io.AddMouseButtonEvent(button, action == GLFW_PRESS);
    if(!io.WantCaptureMouse){
        if(button==GLFW_MOUSE_BUTTON_1 && action==GLFW_PRESS){
            leftCursordown=true;
        }
        else if(button==GLFW_MOUSE_BUTTON_1 && action==GLFW_RELEASE){
            leftCursordown=false;
            firstMouse=true;
        }
    }
}