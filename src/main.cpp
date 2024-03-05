// Copyright 2024 <Mahie Miah>

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <stb_image.h>

#include <iostream>
#include <vector>

#include "IndexBuffer.h"
#include "Program.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include "Texture.h"
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
    glm::vec2 position;
    glm::vec3 color;
    glm::vec2 tex_coords;
};

const std::string &vertexShaderSource = R"(
    #version 460 core
    layout (location = 0) in vec2 position;
    layout (location = 1) in vec3 color;
    layout (location = 2) in vec2 tex_coords;

    uniform vec2 x;
    uniform int scale;
    out vec3 frag_color;
    out vec2 tex_coord;
    
    void main() {
        gl_Position = vec4(position + x, 0.0, 1.0);
        frag_color = color;
        tex_coord = tex_coords * scale;
    }
)";

const std::string &fragmentShaderSource = R"(
    #version 460 core
    in vec3 frag_color;
    in vec2 tex_coord;

    uniform sampler2D tex;
    
    out vec4 color;

    void main() {
        color = texture(tex, tex_coord);
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

    auto window{glfwCreateWindow(800, 800, "I am a Window", nullptr, nullptr)};
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
                   {1,    0, 0},
                   {0,    0}};
    Vertex vertex2{{-0.5, 0.5},
                   {0,    1, 0},
                   {0,    1}};
    Vertex vertex3{{0.5, -0.5},
                   {0,   0, 1},
                   {1,   0}};
    Vertex vertex4{{0.5, 0.5},
                   {1,   1, 1},
                   {1,   1}};

    std::vector shape{vertex1, vertex2, vertex3, vertex4};
    Vertex shape2[]{vertex1, vertex2, vertex3, vertex4};
    GLuint indices[]{0, 1, 2, 1, 2, 3};

    Attribute attributes[] = {{1, offsetof(Vertex, color),      {GL_FLOAT, 3}},
                              {0, offsetof(Vertex, position),   {GL_FLOAT, 2}},
                              {2, offsetof(Vertex, tex_coords), {GL_FLOAT, 2}}};

    VertexBuffer<Vertex> v_buffer{shape};
    IndexBuffer<GLuint> i_buffer{indices};
    VertexArray VAO{v_buffer, i_buffer, attributes};

    glm::vec2 x{1.0f, 1.0f};
    GLint x_location = glGetUniformLocation(program, "x");
    GLint tex_location = glGetUniformLocation(program, "tex");
    GLint scale_location = glGetUniformLocation(program, "scale");

    stbi_set_flip_vertically_on_load(true);
    Texture tex{"assets/pop_cat.png"};
    glBindTextureUnit(0, tex);

    auto resizing = [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    };
    glfwSetFramebufferSizeCallback(window, resizing);

    glm::float32 t{0};
    bool foo = true;
    bool uniform = true;
    glm::int32 scalar{1};

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    //    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    glm::vec4 bg{0.0f};

    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::SliderFloat("Value: t", &t, 0, glm::tau<glm::f32>());
        ImGui::SliderInt("Texture Scale", &scalar, 1, 10);

        ImGui::Checkbox("Draw Shape?", &foo);
        ImGui::Checkbox("Update Uniform?", &uniform);
        ImGui::ColorPicker4("Background Color: ", glm::value_ptr(bg), ImGuiColorEditFlags_PickerHueWheel);

        x.x = glm::sin(5 * t);
        x.y = glm::sin(3 * t);

        glClear(GL_COLOR_BUFFER_BIT);
        glClearColor(bg[0], bg[1], bg[2], bg[3]);
        if (foo) {
            glUseProgram(program);

            if (uniform) {
                glUniform2fv(x_location, 1, glm::value_ptr(x *= 0.5));
                glUniform1i(tex_location, 0);
                glUniform1i(scale_location, scalar);
            }

            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no
            // need to bind it
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
