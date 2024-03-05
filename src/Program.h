//
// Created by mahie on 23/02/2024.
//


#ifndef OPENGLTEMPL_PROGRAM_H
#define OPENGLTEMPL_PROGRAM_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>

struct Diagnostic {
    int success;
    char infoLog[512];
};

class Program {
private:
    GLFWwindow *window;
    GLuint id;
    Diagnostic diagnostic{};

    GLuint create_shader(const std::string &shader_source, GLenum shader_type);

public:
    Program(GLFWwindow *p_window, const std::string &vert_source, const std::string &frag_source);

    operator GLuint() const;
};

#endif // OPENGLTEMPL_PROGRAM_H
