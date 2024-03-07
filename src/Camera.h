//
// Created by mahie on 05/03/2024.
//

#ifndef OPENGLTEMPL_CAMERA_H
#define OPENGLTEMPL_CAMERA_H

#include <glad/glad.h>

#include <GLFW/glfw3.h>

#include <string>

#include "Program.h"
#include "ext/matrix_clip_space.hpp"
#include <glm/ext/matrix_transform.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

class Camera {
public:
  glm::vec3 position;
  glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
  glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

  // Prevents the camera from jumping around when first clicking left click
  bool firstClick = true;

  int width, height;

  float speed = 0.1f;
  float sensitivity = 100.0f;

  Camera(int pwidth, int pheight, glm::vec3 pposition)
      : position(pposition), width(pwidth), height(pheight) {}

  void Matrix(float fov_rad, float z_near, float z_far, Program &program,
              const std::string &uniform) {
    glm::mat4 proj{1.0f};
    glm::mat4 view{1.0f};

    view = glm::lookAt(position, position + orientation, up);
    proj = glm::perspective(fov_rad, static_cast<float>(width) / height, z_near,
                            z_far);

    glUniformMatrix4fv(glGetUniformLocation(program, uniform.c_str()), 1,
                       GL_FALSE, glm::value_ptr(proj * view));
  }

  void inputs(GLFWwindow *window) {

    // Handles key inputs
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
      position += speed * orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
      position += speed * -glm::normalize(glm::cross(orientation, up));
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
      position += speed * -orientation;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
      position += speed * glm::normalize(glm::cross(orientation, up));
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
      position += speed * up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
      position += speed * -up;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
      speed = 0.4f;
    } else if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_RELEASE) {
      speed = 0.1f;
    }
  }
};

#endif //  OPENGLTEMPL_CAMERA_H
