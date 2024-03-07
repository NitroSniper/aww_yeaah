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
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/glm.hpp>

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/vector_angle.hpp>

class Camera {
public:
    glm::vec3 position;
    glm::vec3 orientation = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);

    // Prevents the camera from jumping around when first clicking left click
    bool firstClick = true;

    int width, height;

    float speed = 0.1f;
    float sensitivity = 0.02f;

    Camera(int pwidth, int pheight, glm::vec3 pposition) : position(pposition), width(pwidth), height(pheight) {}

    void Matrix(float fov_rad, float z_near, float z_far, Program &program, const std::string &uniform) {
        glm::mat4 proj{1.0f};
        glm::mat4 view{1.0f};

        view = glm::lookAt(position, position + orientation, up);
        proj = glm::perspective(fov_rad, static_cast<float>(width) / height, z_near, z_far);

        glUniformMatrix4fv(glGetUniformLocation(program, uniform.c_str()), 1, GL_FALSE, glm::value_ptr(proj * view));
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


        // Handles mouse inputs
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_PRESS) {
            // Hides Cursor
            // glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            // get cursor pos

            double x, y;
            // Fetches the coordinates of the cursor
            glfwGetCursorPos(window, &x, &y);

            glm::vec2 foo{x-width / 2, y-height / 2};
            // if the mouse actually moves from the center of the screen.
            if (glm::length(foo) != 0) {
                foo = foo * sensitivity;
                glfwSetCursorPos(window, (width / 2), (height / 2));
                // Calculates upcoming vertical change in the Orientation
                glm::vec3 newOrientation = glm::rotate(orientation, glm::radians(-foo.y), glm::normalize(glm::cross(orientation, up)));
                // Decides whether the next vertical Orientation is legal or not
                if (abs(glm::angle(newOrientation, up) - glm::radians(90.0f)) <= glm::radians(85.0f))
                {
                    orientation = newOrientation;
                }

                // Rotates the Orientation left and right
                orientation = glm::rotate(orientation, glm::radians(-foo.x), up);
            }

        } else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE) == GLFW_RELEASE) {
            // Unhides cursor since camera is not looking around anymore
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            // Makes sure the next time the camera looks around it doesn't jump
            firstClick = true;
        }

    }
};

#endif //  OPENGLTEMPL_CAMERA_H
