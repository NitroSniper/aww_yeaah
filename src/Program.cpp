//
// Created by mahie on 23/02/2024.
//

#include "Program.h"
#include <iostream>

GLuint Program::create_shader(const std::string &shader_source, GLenum shader_type) {

    GLuint shader = glCreateShader(shader_type);
    auto compatible_source = shader_source.c_str(); // errors if the data is not null terminated. but
    // OpenGL does that automatically.
    glShaderSource(shader, 1, &compatible_source, nullptr);
    glCompileShader(shader);
    // check for shader compile errors
    glGetShaderiv(shader, GL_COMPILE_STATUS, &diagnostic.success);
    if (!diagnostic.success) {
        glGetShaderInfoLog(shader, 512, nullptr, diagnostic.infoLog);
        // Should Only be Vert or Frag Shader passed in
        std::string_view shader_type_str{(shader_type == GL_VERTEX_SHADER) ? "VERTEX" : "FRAGMENT"};
        std::cout << "ERROR::SHADER::" << shader_type_str << "::COMPILATION_FAILED\n" << diagnostic.infoLog
                  << std::endl;
    }
    return shader;
}

Program::Program(GLFWwindow *p_window, const std::string &vert_source, const std::string &frag_source) : window(
        p_window) {
    auto vertex = create_shader(vert_source, GL_VERTEX_SHADER);
    auto fragment = create_shader(frag_source, GL_FRAGMENT_SHADER);
    id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    glGetProgramiv(id, GL_LINK_STATUS, &diagnostic.success);
    if (!diagnostic.success) {
        glGetProgramInfoLog(id, 512, nullptr, diagnostic.infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << diagnostic.infoLog << std::endl;
    }
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

Program::operator GLuint() const { return id; }
