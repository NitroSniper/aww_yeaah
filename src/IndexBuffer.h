//
// Created by mahie on 29/02/2024.
//

#ifndef OPENGLTEMPL_INDEXBUFFER_H
#define OPENGLTEMPL_INDEXBUFFER_H

#include <span>
#include <glad/glad.h>

template<typename T>
class IndexBuffer {
private:
    GLuint id_{};
    std::span<T> data_;
    GLenum draw_mode{GL_TRIANGLES};

public:
    explicit IndexBuffer(const std::span<T> &data);
    virtual ~IndexBuffer();

    GLsizei get_size() const {
        return data_.size();
    }
    GLenum get_draw_mode() const {
        return draw_mode;
    }
    operator GLuint() const {return id_;}
};

template<typename T>
IndexBuffer<T>::IndexBuffer(const std::span<T> &data) : data_(data) {
    glCreateBuffers(1, &id_);
    glNamedBufferData(id_, data.size_bytes(), &data.front(), GL_STATIC_DRAW);
}

template<typename T>
IndexBuffer<T>::~IndexBuffer() {
    glDeleteBuffers(1, &id_);
}



#endif //OPENGLTEMPL_INDEXBUFFER_H
