
#ifndef FRAMEBUFFER_H
#define FRAMEBUFFER_H

#include "texture.h"

#include <stdbool.h>

typedef struct {
    int w, h;
    int numAttachments;
    unsigned int colorAttachments[8];
    unsigned int dsTex;
    unsigned int fbo;
} framebuffer;

void initFramebuffer(framebuffer* fb, int w, int h);
void addColorAttachment(framebuffer* fb, bool vec);
void completeFramebuffer(framebuffer* fb);

void renderToFramebuffer(framebuffer* fb);
void useFramebuffer(framebuffer* fb);

#endif
