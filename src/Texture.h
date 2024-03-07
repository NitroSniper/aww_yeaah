//
// Created by mahie on 05/03/2024.
//

#ifndef OPENGLTEMPL_TEXTURE_H
#define OPENGLTEMPL_TEXTURE_H


#include <string>
#include <stb_image.h>
#include <glad/glad.h>

class Texture {
private:
    int width, height, numColChannel;
    GLuint id_;
public:
    explicit Texture(const std::string &path, GLenum tex_type);

    virtual ~Texture();

    operator GLuint() const;
};


#endif //OPENGLTEMPL_TEXTURE_H
