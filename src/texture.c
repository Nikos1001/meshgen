
#include "texture.h"

#include "util.h"

#define GL_SILENCE_DEPRECATION
#include <GL/glew.h>

#define STB_IMAGE_IMPLEMENTATION
#include "../lib/stb_image.h"

texture loadTexture(const char* path) {
    texture res;
    int nrChannels;
    unsigned char* data = stbi_load(path, &res.w, &res.h, &nrChannels, 0);
    if(data == NULL) {
        FATAL_ERROR("Could not load texture %s", path);
    }
    glGenTextures(1, &res.tex);
    glBindTexture(GL_TEXTURE_2D, res.tex);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, nrChannels == 4 ? GL_RGBA : GL_RGB, res.w, res.h, 0, nrChannels == 4 ? GL_RGBA : GL_RGB, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return res;
}

void useTexture(texture* t, int slot) {
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, t->tex);
}
