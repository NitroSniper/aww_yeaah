//
// Created by mahie on 01/03/2024.
//

#ifndef OPENGLTEMPL_VERTEXARRAY_H
#define OPENGLTEMPL_VERTEXARRAY_H

#include <cstddef>
#include <glad/glad.h>

#include "Camera.h"
#include "IndexBuffer.h"
#include "Program.h"
#include "Texture.h"
#include "VertexBuffer.h"

#include <iostream>
#include <string>
#include <utility>

struct Attribute {
    GLuint attrib_index;
    size_t offset;
    std::pair<GLenum, GLint> type_size;
};

template<typename T, typename U>
class VertexArray {
private:
    GLuint id_{};
    std::span<Texture> textures;
    IndexBuffer<U> ibo_;

public:
    VertexArray(VertexBuffer<T> vbo, IndexBuffer<U> ibo, std::span<Attribute> attribs, std::span<Texture> tex)
            : textures(tex), ibo_(ibo) {
        glCreateVertexArrays(1, &id_);

        for (int i = 0; i < attribs.size(); ++i) {
            Attribute attrib{attribs[i]};
            glEnableVertexArrayAttrib(id_, attrib.attrib_index);
            glVertexArrayAttribBinding(id_, attrib.attrib_index, 0);
            glVertexArrayAttribFormat(id_, attrib.attrib_index, attrib.type_size.second, attrib.type_size.first,
                                      GL_FALSE, attrib.offset);
        }

        glVertexArrayVertexBuffer(id_, 0, vbo, 0, vbo.stride);
        glVertexArrayElementBuffer(id_, ibo_);
    }

    ~VertexArray() { glDeleteVertexArrays(1, &id_); }

    void draw(const Program &program, const Camera &camera) {
        GLuint diff_i{0};
        GLuint spec_i{0};
        // Somehow picking a texture fucks the entire program, I don't konw what causes this. this is so fucking stupid
        // auto tex = &textures[0];
        for (Texture& tex: textures) {
            std::string tex_name;
            GLuint i = diff_i + spec_i;
            switch (tex.type) {
                case Texture::TextureType::DIFFUSE:
                    tex_name = "diff_" + std::to_string(diff_i++);
                    break;
                case Texture::TextureType::SPECULAR:
                    tex_name = "spec_" + std::to_string(spec_i++);
                    break;
            }
            tex.bind(i);
            glUniform1i(glGetUniformLocation(program, tex_name.c_str()), i);

        }

        camera.uniform(program, "camera");
        glBindVertexArray(id_);
        glDrawElements(ibo_.get_draw_mode(), ibo_.get_size(), GL_UNSIGNED_INT, nullptr);
    }

    operator GLuint() const { return id_; }
};

#endif // OPENGLTEMPL_VERTEXARRAY_H
