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
                   TextureType tex_type)     : type(tex_type) {
    stbi_uc *bytes = stbi_load(path.c_str(), &width, &height, &numColChannel, 0);
    glCreateTextures(GL_TEXTURE_2D, 1, &id_);

    glTextureParameteri(id_, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTextureParameteri(id_, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTextureParameteri(id_, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTextureStorage2D(id_, 1, GL_RGBA8, width, height);
    glTextureSubImage2D(id_, 0, 0, 0, width, height, format, GL_UNSIGNED_BYTE,
                        bytes);
    // RGB for jpeg, RGBA for png
    glGenerateTextureMipmap(id_);
    stbi_image_free(bytes);
  };

  virtual ~Texture() { glDeleteTextures(1, &id_); } ;
  void bind(GLuint unit) const { glBindTextureUnit(unit, id_); };

  operator GLuint() const { return id_; };
};

#endif // OPENGLTEMPL_TEXTURE_H
