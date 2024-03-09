//
// Created by mahie on 01/03/2024.
//

#ifndef OPENGLTEMPL_VERTEXARRAY_H
#define OPENGLTEMPL_VERTEXARRAY_H

#include <cstddef>
#include <glad/glad.h>

#include "IndexBuffer.h"
#include "VertexBuffer.h"

#include <utility>

struct Attribute {
  GLuint attrib_index;
  size_t offset;
  std::pair<GLenum, GLint> type_size;
};

template <typename T, typename U> class VertexArray {
private:
  GLuint id_{};

public:
  VertexArray(VertexBuffer<T> vbo, IndexBuffer<U> ibo,
              std::span<Attribute> attribs, size_t stride) {
    glCreateVertexArrays(1, &id_);

    for (int i = 0; i < attribs.size(); ++i) {
      Attribute attrib{attribs[i]};
      glEnableVertexArrayAttrib(id_, attrib.attrib_index);
      glVertexArrayAttribBinding(id_, attrib.attrib_index, 0);
      glVertexArrayAttribFormat(id_, attrib.attrib_index,
                                attrib.type_size.second, attrib.type_size.first,
                                GL_FALSE, attrib.offset);
    }

    glVertexArrayVertexBuffer(id_, 0, vbo, 0, stride);
    glVertexArrayElementBuffer(id_, ibo);
  }
  ~VertexArray() { glDeleteVertexArrays(1, &id_); }
  operator GLuint() const { return id_; }
};

#endif // OPENGLTEMPL_VERTEXARRAY_H
