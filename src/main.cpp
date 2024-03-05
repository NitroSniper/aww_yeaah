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
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 tex_coords;
};

const std::string &vertexShaderSource = R"(
    #version 460 core
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 color;
    layout (location = 2) in vec2 tex_coords;

    uniform mat4 model;
    uniform mat4 proj;
    uniform mat4 view;

    uniform int scale;

    out vec3 frag_color;
    out vec2 tex_coord;
    
    void main() {
        gl_Position = proj * view * model * vec4(position, 1.0);
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


    int OpenGLVersion[2];
    glGetIntegerv(GL_MAJOR_VERSION, &OpenGLVersion[0]);
    glGetIntegerv(GL_MINOR_VERSION, &OpenGLVersion[1]);
    std::cout << "OpenGL Version: " << OpenGLVersion[0] << '.' << OpenGLVersion[1] << std::endl;
    Program program = Program(window, vertexShaderSource, fragmentShaderSource);

    Vertex vertex1{{-0.5, 0.0, 0.5},
                   {1,    0, 0},
                   {0,    0}};
    Vertex vertex2{{-0.5, 0.0, -0.5},
                   {0,    1, 0},
                   {5,    0}};
    Vertex vertex3{{0.5, 0.0, -0.5},
                   {0,   0, 1},
                   {0,   0}};
    Vertex vertex4{{0.5, 0.0, 0.5},
                   {1,   1, 1},
                   {5,   0}};
    Vertex vertex5{{0.0, 0.8, 0.0},
                   {0.83,    0.70, 0.44},
                   {2.5,   2.5}};

    std::vector shape{vertex1, vertex2, vertex3, vertex4, vertex5};
    Vertex shape2[]{vertex1, vertex2, vertex3, vertex4, vertex5};
    GLuint indices[]{0, 1, 2, 0, 2, 3, 0, 1, 4, 1, 2, 4, 2, 3, 4, 3, 0, 4};

    Attribute attributes[] = {{1, offsetof(Vertex, color),      {GL_FLOAT, 3}},
                              {0, offsetof(Vertex, position),   {GL_FLOAT, 3}},
                              {2, offsetof(Vertex, tex_coords), {GL_FLOAT, 2}}};

    VertexBuffer<Vertex> v_buffer{shape};
    IndexBuffer<GLuint> i_buffer{indices};
    VertexArray VAO{v_buffer, i_buffer, attributes};

    glm::vec2 x{1.0f, 1.0f};

    stbi_set_flip_vertically_on_load(true);
    Texture tex{"assets/pop_cat.png"};
    glBindTextureUnit(0, tex);


    auto resizing = [](GLFWwindow *window, int width, int height) {
        glViewport(0, 0, width, height);
    };
    glfwSetFramebufferSizeCallback(window, resizing);

    GLint tex_location = glGetUniformLocation(program, "tex");
    GLint scale_location = glGetUniformLocation(program, "scale");
    GLint model_location = glGetUniformLocation(program, "model");
    GLint proj_location = glGetUniformLocation(program, "proj");
    GLint view_location = glGetUniformLocation(program, "view");



    glm::float32 t{0};
    bool foo = true;
    bool uniform = true;
    glm::int32 scalar{1};
    glm::vec3 translation{0.0f};
    glm::float32 z_near{0.1};
    glm::float32 z_far{100.0};
    glm::float32 rotation{0};

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO &io = ImGui::GetIO();
    //    (void) io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 460");

    glm::vec4 bg{0.0f};

    // glEnable(GL_FRAMEBUFFER_SRGB); Gamma correction
    glEnable(GL_DEPTH_TEST);
    glUseProgram(program);
    while (!glfwWindowShouldClose(window)) {
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();

        ImGui::NewFrame();
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
        ImGui::SliderFloat("Value: t", &t, 0, glm::tau<glm::f32>());
        ImGui::SliderInt("Texture Scale", &scalar, 1, 10);
        ImGui::SliderFloat3("Translation Vector", glm::value_ptr(translation), -5, 5);
        ImGui::SliderFloat("Rotation", &rotation, 0, glm::tau<glm::f32>());

        ImGui::SliderFloat("zNear:", &z_near, 0, 100);
        ImGui::SliderFloat("zFar:", &z_far, 0, 100);

        ImGui::Checkbox("Draw Shape?", &foo);
        ImGui::Checkbox("Update Uniform?", &uniform);
        ImGui::ColorPicker4("Background Color: ", glm::value_ptr(bg), ImGuiColorEditFlags_PickerHueWheel);

        x.x = glm::sin(5 * t);
        x.y = glm::sin(3 * t);

        glm::mat4 model{1.0f};
        glm::mat4 view{1.0f};
        glm::mat4 proj{1.0f}; // THE PERSPECTIVE MATRIX (╯°□°）╯︵ ┻━┻
        view = glm::translate(view, translation);
        proj = glm::perspective(glm::radians(45.0f), 800.0f/800.0f, z_near, z_far);
        model = glm::rotate(model, rotation, glm::vec3(0.0, 1.0f, 0.0f));


        // Drawing
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glClearColor(bg[0], bg[1], bg[2], bg[3]);
        if (uniform) {
            glUniform1i(tex_location, 0);
            glUniform1i(scale_location, scalar);
            glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
            glUniformMatrix4fv(proj_location, 1, GL_FALSE, glm::value_ptr(proj));
            glUniformMatrix4fv(view_location, 1, GL_FALSE, glm::value_ptr(view));
        }
        if (foo) {

            glBindVertexArray(VAO); // seeing as we only have a single VAO there's no
            // need to bind it
            glDrawElements(GL_TRIANGLES, sizeof(indices) / sizeof(GLuint), GL_UNSIGNED_INT, nullptr);
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
