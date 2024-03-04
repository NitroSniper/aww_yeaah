// Copyright 2024 <Mahie Miah>


#include <glad/glad.h>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
#include <vector>
#include "Program.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include <imgui.h>
#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>

struct Vertex {
    float position[2];
    float color[3];
};

const std::string &vertexShaderSource = R"(
    #version 460 core
    layout (location = 1) in vec3 color;
    layout (location = 0) in vec2 position;

    uniform vec2 x;
    out vec3 frag_color;
    
    void main() {
        vec2 new_pos = position;
        new_pos += x;
        gl_Position = vec4(new_pos, 0.0, 1.0);
        frag_color = color;
    }
)";

const std::string &fragmentShaderSource = R"(
    #version 460 core
    in vec3 frag_color;
    out vec4 color;
    
    void main() {
        color = vec4(frag_color, 1.0);
    }
)";

static void glClearError() {
    while (glGetError()) {
    }
}

static void glCheckError() {
    while (GLenum error = glGetError()) {
        std::cout << "[OpenGL Error] (" << error << ')' << std::endl;

    }
}


struct Triangle {

};

int main(int, char **) {

    if (!glfwInit()) {
        std::cerr << "GLFW Could not be initialised." << std::endl;
        return -1;
    }

    auto cleanup = []() {
        glfwTerminate();
        std::cout << "GLFW has terminated." << std::endl;
    };
    std::atexit(cleanup);

    auto window{glfwCreateWindow(960, 540, "I am a Window", nullptr, nullptr)};
    glfwMakeContextCurrent(window);


    // Check if glad is loaded
    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cerr << "Couldn't load OpenGL" << std::endl;
        return EXIT_FAILURE;
    }
    // glEnable(GL_FRAMEBUFFER_SRGB); Gamma correction



    int OpenGLVersion[2];
    glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
    std::cout << "OpenGL Version: " << OpenGLVersion[0] << '.' << OpenGLVersion[1] << std::endl;
    Program program = Program(window, vertexShaderSource, fragmentShaderSource);

    Vertex vertex1{{-0.5, -0.5},
                   {1,    0, 0}};
    Vertex vertex2{{-0.5, 0.5},
                   {0,    1, 0}};
    Vertex vertex3{{0.5, -0.5},
                   {0,   0, 1}};
    Vertex vertex4{{0.5, 0.5},
                   {1,   1, 1}};

    std::vector shape{vertex1, vertex2, vertex3, vertex4};
    Vertex shape2[]{vertex1, vertex2, vertex3, vertex4};
    GLuint indices[]{0, 1, 2, 1, 2, 3};


    Attribute attributes[] = {{1, offsetof(Vertex, color),    {GL_FLOAT, 3}},
                              {0, offsetof(Vertex, position), {GL_FLOAT, 2}}};

    VertexBuffer<Vertex> v_buffer{shape};
    IndexBuffer<GLuint> i_buffer{indices};
    VertexArray VAO{v_buffer, i_buffer, attributes};

    glm::vec2 x{1.0f, 1.0f};
    GLint x_location = glGetUniformLocation(program, "x");

    // Issue caused by not binding program before using it.
    // glUniform2fv(x_location, 1, glm::value_ptr(x)); // causes OPENGL 1282

    // used to resize window
    auto resizing = [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    };
    glfwSetFramebufferSizeCallback(window, resizing);

    glm::float32 t{0};

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
//    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    bool foo = true;


    while (!glfwWindowShouldClose(window)) {

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::SliderFloat("Value: t", &t, 0, 10);
        ImGui::Checkbox("what", &foo);

        x.x = glm::sin(5 * t);
        x.y = glm::sin(3 * t);


        glClear(GL_COLOR_BUFFER_BIT);
        if (foo) {
            glUseProgram(program);
            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no need to bind it
            glUniform2fv(x_location, 1, glm::value_ptr(x *= 0.5));
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr);
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());


        glfwPollEvents();
        glfwSwapBuffers(window);
        glCheckError();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();
}
