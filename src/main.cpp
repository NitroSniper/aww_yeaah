// Copyright 2024 <Mahie Miah>
#define GLM_ENABLE_EXPERIMENTAL

#include <backends/imgui_impl_glfw.h>
#include <backends/imgui_impl_opengl3.h>
#include <glad/glad.h>
#include <imgui.h>
#include <stb_image.h>

#include <iostream>
#include <vector>

#include "Camera.h"
#include "IndexBuffer.h"
#include "Program.h"
#include "Texture.h"
#include "VertexArray.h"
#include "VertexBuffer.h"
#include <cstddef>
#include <cstdlib>
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Vertex {
    glm::vec3 position;
    glm::vec3 color;
    glm::vec2 tex_coords;
    glm::vec3 normal;
};
struct Vertex2 {
    glm::vec3 position;
};

const std::string &vertexShaderSource = R"(
    #version 460 core
    layout (location = 0) in vec3 position;
    layout (location = 1) in vec3 color;
    layout (location = 2) in vec2 tex_coords;
    layout (location = 3) in vec3 normal;

    uniform mat4 model;
    uniform mat4 camera;
    uniform int scale;

    out vec3 frag_color;
    out vec2 tex_coord;

    out vec3 Normal;
    out vec3 crntPos;
    
    void main() {
        crntPos = vec3(model * vec4(position, 1.0f));
        gl_Position = camera * vec4(crntPos, 1.0);
        frag_color = color;
        tex_coord = tex_coords * scale;
        Normal = normal;
    }
)";

const std::string &fragmentShaderSource = R"(
    #version 460 core
    in vec3 frag_color;
    in vec2 tex_coord;
    in vec3 Normal;
    in vec3 crntPos;

    uniform sampler2D diff_0;
    uniform sampler2D spec_0;
    uniform vec4 light_color;
    uniform vec3 light_pos;
    uniform vec3 camera_pos;
    
    // lighting function
    uniform float a;
    uniform float b;

    out vec4 color;


    vec4 point_light() {
        vec3 lightVec = light_pos - crntPos;
        float dist = length(lightVec);

        float inten = 1 / ((a * dist + b) * dist + 1);

        float ambient = 0.2;

        vec3 normal = normalize(Normal);
        vec3 light_direction = normalize(lightVec);

        float diffuse = max(dot(normal, light_direction), 0);

        float spec_light = 0.5;
        vec3 view_direction = normalize(camera_pos - crntPos);
        vec3 reflection = reflect(-light_direction, normal);
        float spec_amount = pow(max(dot(view_direction, reflection), 0), 8);
        float specular = spec_light * spec_amount;

        return (texture(diff_0, tex_coord) * (diffuse * inten + ambient) + texture(spec_0, tex_coord).r * specular*inten) * light_color;
    }

//    vec4 direct_light() {
//        vec3 lightVec = vec3(1, 1, 0);
//
//        float ambient = 0.2;
//
//        vec3 normal = normalize(Normal);
//        vec3 light_direction = normalize(lightVec);
//        float diffuse = max(dot(normal, light_direction), 0);
//
//        float spec_light = 0.5;
//        vec3 view_direction = normalize(camera_pos - crntPos);
//        vec3 reflection = reflect(-light_direction, normal);
//        float spec_amount = pow(max(dot(view_direction, reflection), 0), 8);
//        float specular = spec_light * spec_amount;
//
//        return (texture(diff0, tex_coord) * (diffuse + ambient) + texture(spec0, tex_coord).r * specular) * light_color;
//    }
//
//    vec4 spot_light() {
//        float innerCone = a;
//        float outerCone = b;
//
//
//        vec3 lightVec = light_pos - crntPos;
//
//        float ambient = 0.2;
//
//        vec3 normal = normalize(Normal);
//        vec3 light_direction = normalize(lightVec);
//
//        float diffuse = max(dot(normal, light_direction), 0);
//
//        float spec_light = 0.5;
//        vec3 view_direction = normalize(camera_pos - crntPos);
//        vec3 reflection = reflect(-light_direction, normal);
//        float spec_amount = pow(max(dot(view_direction, reflection), 0), 8);
//        float specular = spec_light * spec_amount;
//
//        float angle = dot(vec3(0, -1, 0), -light_direction);
//        float inten = clamp((angle - outerCone)/(innerCone - outerCone), 0, 1);
//
//        return (texture(diff0, tex_coord) * (diffuse * inten + ambient) + texture(spec0, tex_coord).r * specular * inten) * light_color;
//    }

    void main() {
        color = point_light();
    }
)";

const std::string &light_vert = R"(
    #version 460 core
    layout (location = 0) in vec3 position;

    uniform mat4 model;
    uniform mat4 camera;

    void main() {
        gl_Position = camera * model * vec4(position, 1.0);
    }
)";

const std::string &light_frag = R"(
    #version 460 core

    out vec4 color;
    uniform vec4 in_color;

    void main() {
        color = in_color;
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

constexpr GLint width = 1920;
constexpr GLint height = 1080;

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

  auto window{glfwCreateWindow(width, height, "I am a Window", nullptr, nullptr)};
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
  stbi_set_flip_vertically_on_load(true);

  Program program = Program(window, vertexShaderSource, fragmentShaderSource);
  Program light_program = Program(window, light_vert, light_frag);


  GLfloat vertices[] = {
          //     COORDINATES     /        COLORS        /    TexCoord    / NORMALS
          //     //
          -1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 0.0f,
          1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
          0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f};
  GLuint indices[] = {0, 1, 2, 0, 2, 3};
  Attribute attributes[] = {{0, 0,                   {GL_FLOAT, 3}},
                            {1, sizeof(GLfloat) * 3, {GL_FLOAT, 3}},
                            {2, sizeof(GLfloat) * 6, {GL_FLOAT, 2}},
                            {3, sizeof(GLfloat) * 8, {GL_FLOAT, 3}}};
  Texture tex0{"assets/planks.png", GL_RGBA, Texture::TextureType::DIFFUSE};
  Texture tex1{"assets/planksSpec.png", GL_RED, Texture::TextureType::SPECULAR};
  Texture textures[]{tex0, tex1};

  VertexBuffer<GLfloat> v_buffer{vertices, sizeof(GLfloat) * 11};
  IndexBuffer<GLuint> i_buffer{indices};
  VertexArray floor{v_buffer, i_buffer, attributes, textures};


  Vertex2 lightVertices[] = {//     COORDINATES     //
          {{-0.1f, -0.1f, 0.1f}},
          {{-0.1f, -0.1f, -0.1f}},
          {{0.1f,  -0.1f, -0.1f}},
          {{0.1f,  -0.1f, 0.1f}},
          {{-0.1f, 0.1f,  0.1f}},
          {{-0.1f, 0.1f,  -0.1f}},
          {{0.1f,  0.1f,  -0.1f}},
          {{0.1f,  0.1f,  0.1f}}};

  GLuint lightIndices[] = {0, 1, 2, 0, 2, 3, 0, 4, 7, 0, 7, 3, 3, 7, 6, 3, 6, 2, 2, 6, 5, 2, 5, 1, 1, 5, 4, 1, 4, 0, 4,
                           5, 6, 4, 6, 7};

  Attribute light_attr[] = {{0, offsetof(Vertex2, position), {GL_FLOAT, 3}}};
  VertexBuffer<Vertex2> light_vertices{lightVertices, sizeof(Vertex2)};
  IndexBuffer<GLuint> light_indices{lightIndices};
  VertexArray light_cube{light_vertices, light_indices, light_attr, {}};
  glm::vec4 light_color{1.0f, 1.0f, 1.0f, 1.0f};
  glm::vec3 light_pos = glm::vec3(0.5, 0.5, 0.5);

  Camera camera(width, height, glm::vec3(0.0f, 0.5f, 2.0f));


  auto resizing = [](GLFWwindow *window, int width, int height) {
      glViewport(0, 0, width, height);
  };
  glfwSetFramebufferSizeCallback(window, resizing);

  bool foo = true;
  bool uniform = true;
  bool rotate = true;
  glm::float32 t{};
  glm::int32 scalar{1};
  glm::float32 z_near{0.1};
  glm::float32 z_far{100};
  glm::float32 rotation{0};
  glm::vec4 bg{0};
  glm::int32 fov{45};
  glm::float32 a{3};
  glm::float32 b{0.7};

  GLint tex_location = glGetUniformLocation(program, "tex");
  GLint scale_location = glGetUniformLocation(program, "scale");
  GLint model_location = glGetUniformLocation(program, "model");
  GLint lmodel_location = glGetUniformLocation(light_program, "model");

  IMGUI_CHECKVERSION();
  ImGui::CreateContext();
  ImGuiIO &io = ImGui::GetIO();
  //    (void) io;
  ImGui::StyleColorsDark();
  ImGui_ImplGlfw_InitForOpenGL(window, true);
  ImGui_ImplOpenGL3_Init("#version 460");
  // glEnable(GL_FRAMEBUFFER_SRGB); // Gamma correction
  glEnable(GL_DEPTH_TEST);
  while (!glfwWindowShouldClose(window)) {
    // Create Imgui
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
    ImGui::SliderInt("Tex Scale", &scalar, 1, 10);
    ImGui::SliderInt("Fov", &fov, 1, 180);
    ImGui::SliderFloat3("Light Pos", glm::value_ptr(light_pos), -5, 5);
    ImGui::Checkbox("Rotate?", &rotate);
    ImGui::SliderFloat("Angle", &rotation, 0, glm::tau<glm::f32>());
    t += 0.005;

    ImGui::SliderFloat("zNear", &z_near, 0, 100);
    ImGui::SliderFloat("zFar", &z_far, 0, 100);

    ImGui::Checkbox("Draw Shape?", &foo);
    ImGui::Checkbox("Update Uniform?", &uniform);
    ImGui::ColorPicker4("Background Color: ", glm::value_ptr(bg), ImGuiColorEditFlags_PickerHueWheel);
    ImGui::SliderFloat("Light: Param A", &a, 0, 3);
    ImGui::SliderFloat("Light: Param B", &b, 0, 3);

    // Create and update model
    glm::mat4 model{1.0f};
    glm::mat4 light_model{1.0f};
    model = glm::rotate(model, rotation, glm::vec3(0.0, 1.0f, 0.0f));
    light_model = glm::translate(light_model, light_pos);
    light_model = glm::rotate(light_model, rotation, glm::vec3(0.0, 1.0f, 0.0f));

    camera.update_matrix(glm::radians(static_cast<float>(fov)), z_near, z_far);
    camera.inputs(window);

    // Drawing
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(bg[0], bg[1], bg[2], bg[3]);
    if (foo) {
      glUseProgram(program);
      glUniform1i(scale_location, scalar);
      glUniformMatrix4fv(model_location, 1, GL_FALSE, glm::value_ptr(model));
      glUniform4fv(glGetUniformLocation(program, "light_color"), 1, glm::value_ptr(light_color));
      glUniform3fv(glGetUniformLocation(program, "light_pos"), 1, glm::value_ptr(light_pos));
      glUniform1f(glGetUniformLocation(program, "a"), a);
      glUniform1f(glGetUniformLocation(program, "b"), b);
      glUniform3fv(glGetUniformLocation(program, "camera_pos"), 1, glm::value_ptr(camera.position));
      floor.draw(program, camera);


      glUseProgram(light_program);
      glUniformMatrix4fv(glGetUniformLocation(light_program, "model"), 1, GL_FALSE, glm::value_ptr(light_model));
      glUniform4fv(glGetUniformLocation(light_program, "in_color"), 1, glm::value_ptr(light_color));
      light_cube.draw(light_program, camera);
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
