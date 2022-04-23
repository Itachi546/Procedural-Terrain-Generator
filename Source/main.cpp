#include "ogl.h"
#include "camera.h"
#include "input.h"
#include "terrain/terrain.h"

#include <GLFW/glfw3.h>
#include <iostream>

/**************************************************************************************************************/

float gOGLVersion;

struct State
{
  int width;
  int height;
} gState;

struct PerFrameData
{
    glm::mat4 projection;
    glm::mat4 view;
    glm::mat4 VP;
} gPerFrameData;

FirstPersonCamera camera(glm::vec3(0.0f, 1.0f, 2.0f), glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, 1.0f, 0.0f));

/**************************************************************************************************************/
// Callback

void GLAPIENTRY
MessageCallback(GLenum source,  GLenum type,  GLuint id, GLenum severity, GLsizei length,  const GLchar* message, const void* userParam)
{
    fprintf(stderr, "GL CALLBACK: source = 0x%x %s type = 0x%x, severity = 0x%x, message = %s\n",
        source,
        (type == GL_DEBUG_TYPE_ERROR ? "** GL ERROR **" : ""),
		type, severity, message);
}

void GLFWErrorCallback(int errorCode, const char* description)
{
    std::cout << "GLFWError(ErrorCode: [" << errorCode << "]) " << description << std::endl;
}

/**************************************************************************************************************/

void WindowResizeCb(GLFWwindow* window, int width, int height) {

    gState.width  = width;
    gState.height = height;

    if (width == 0 && height == 0)
        return;
    camera.setAspect(width / (float)height);
}

void KeyCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    bool isDown = action != GLFW_RELEASE;

    if (key == GLFW_KEY_ESCAPE && isDown)
        glfwSetWindowShouldClose(window, true);


    Input::SetState(key, isDown, mods);
}

void MouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    Input::SetState(button, action != GLFW_RELEASE, mods);
}

void CursorPositionCb(GLFWwindow* window, double x, double y)
{
    MouseState::SetMousePosition(static_cast<float>(x), static_cast<float>(y));
}

/**************************************************************************************************************/
int main()
{
    gState.width = 800;
    gState.height = 600;

    if (!glfwInit())
		glfwInit();

    glfwSetErrorCallback(GLFWErrorCallback);

    GLFWwindow* window = glfwCreateWindow(gState.width, gState.height, "Hello World", 0, 0);

    if(window == nullptr)
    {
        fprintf(stderr, "Failed to Create Window");
        return 0;
    }

    glfwMakeContextCurrent(window);
    
    // Set GLFW Callback
	glfwSetWindowSizeCallback(window, WindowResizeCb);
    glfwSetKeyCallback(window, KeyCallback);
    glfwSetMouseButtonCallback(window, MouseCallback);
    glfwSetCursorPosCallback(window, CursorPositionCb);
	glfwSwapInterval(1);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

    // Enable Debug Output
    glEnable(GL_DEBUG_OUTPUT);
    glDebugMessageCallback(MessageCallback, 0);

    // Get OGL Version
    std::string version = (const char*)glGetString(GL_VERSION);

    std::cout << version << std::endl;
    std::cout << glGetString(GL_RENDERER) << std::endl;
    std::cout << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;

    gOGLVersion = (float)std::atof(version.substr(0, 3).c_str());

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

    // Global Uniform Buffer
    GLBuffer perFrameDataBuffer(nullptr, sizeof(PerFrameData), GL_DYNAMIC_STORAGE_BIT);
    glBindBufferRange(GL_UNIFORM_BUFFER, 0, perFrameDataBuffer.getHandle(), 0, sizeof(PerFrameData));

    // Terrain
    std::shared_ptr<Terrain> terrain = std::make_shared<Terrain>(31, 1);

    float dt = 0.016f;
    float startTime = static_cast<float>(glfwGetTime());
    bool wireframe = true;

    while(!glfwWindowShouldClose(window))
    {
        if (Input::IsKeyDown(GLFW_KEY_SPACE))
            wireframe = !wireframe;

        if (wireframe)
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        else
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glViewport(0, 0, gState.width, gState.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update Camera
        camera.update(dt);
        terrain->update(&camera, dt);

        // Update PerFrameData
        gPerFrameData.projection = camera.getProjectionMatrix();
        gPerFrameData.view = camera.getViewMatrix();
        gPerFrameData.VP = gPerFrameData.projection * gPerFrameData.view;
        glNamedBufferSubData(perFrameDataBuffer.getHandle(), 0, sizeof(PerFrameData), &gPerFrameData);

        // Draw
        terrain->draw();

        glfwPollEvents();
        glfwSwapBuffers(window);

        float endTime = static_cast<float>(glfwGetTime());
        dt = endTime - startTime;
        startTime = endTime;

        std::string title = "frameTime: " + std::to_string(dt) + "ms";
        glfwSetWindowTitle(window, title.c_str());
    }

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
