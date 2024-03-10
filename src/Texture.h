//
// Created by mahie on 05/03/2024.
//

#ifndef OPENGLTEMPL_TEXTURE_H
#define OPENGLTEMPL_TEXTURE_H

#include <glad/glad.h>
#include <stb_image.h>
#include <string>

class Texture {
private:
  int width, height, numColChannel;
  GLuint id_;

public:
  enum class TextureType { DIFFUSE, SPECULAR };
  TextureType type;

  explicit Texture(const std::string &path, GLenum format,
                   TextureType tex_type);

  virtual ~Texture();
  void bind(GLuint unit);

  operator GLuint() const;
};

#endif // OPENGLTEMPL_TEXTURE_H
