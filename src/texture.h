
#ifndef TEXTURE_H
#define TEXTURE_H

typedef struct {
    int w, h;
    unsigned int tex;
} texture;

texture loadTexture(const char* path);
void useTexture(texture* t, int slot);

#endif
