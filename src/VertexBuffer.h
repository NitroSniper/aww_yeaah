//
// Created by mahie on 01/03/2024.
//

#include <span>
#include <glad/glad.h>

#ifndef OPENGLTEMPL_VERTEXBUFFER_H
#define OPENGLTEMPL_VERTEXBUFFER_H


template<typename T>
class VertexBuffer {
private:
    GLuint id_{};
    std::span<T> data_;
public:
    explicit VertexBuffer(const std::span<T> &data);
    virtual ~VertexBuffer();

    operator GLuint() const {return id_;}
};

template<typename T>
VertexBuffer<T>::VertexBuffer(const std::span<T> &data) : data_(data) {
    glCreateBuffers(1, &id_);
    glNamedBufferData(id_, data.size_bytes(), &data.front(), GL_STATIC_DRAW);
}

template<typename T>
VertexBuffer<T>::~VertexBuffer() {
    glDeleteBuffers(1, &id_);
}


#endif //OPENGLTEMPL_VERTEXBUFFER_H
