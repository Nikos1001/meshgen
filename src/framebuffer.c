
#include "framebuffer.h"
#include "util.h"

#include <GL/glew.h>

#include <stdlib.h>

void initFramebuffer(framebuffer* fb, int w, int h) {
    glGenFramebuffers(1, &fb->fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);

    fb->numAttachments = 0;
    
    // glGenTextures(1, &fb->tex.tex);
    // glBindTexture(GL_TEXTURE_2D, fb->tex.tex);
    // glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, fb->tex.tex, 0);
    // glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &fb->dsTex);
    glBindTexture(GL_TEXTURE_2D, fb->dsTex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, w, h, 0, GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, fb->dsTex, 0);

    fb->w = w;
    fb->h = h;

}

void addColorAttachment(framebuffer* fb, bool vec) {
    glGenTextures(1, &fb->colorAttachments[fb->numAttachments]);
    glBindTexture(GL_TEXTURE_2D, fb->colorAttachments[fb->numAttachments]);
    glTexImage2D(GL_TEXTURE_2D, 0, vec ? GL_RGBA16F : GL_RGBA, fb->w, fb->h, 0, GL_RGBA, vec ? GL_FLOAT : GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0 + fb->numAttachments, GL_TEXTURE_2D, fb->colorAttachments[fb->numAttachments], 0);
    fb->numAttachments++;
}

void completeFramebuffer(framebuffer* fb) {

    unsigned int attachments[fb->numAttachments];
    for(int i = 0; i < fb->numAttachments; i++)
        attachments[i] = GL_COLOR_ATTACHMENT0 + i;
    glDrawBuffers(fb->numAttachments, attachments);

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        FATAL_ERROR("Framebuffer not complete.");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}



void renderToFramebuffer(framebuffer* fb) {
    glBindFramebuffer(GL_FRAMEBUFFER, fb->fbo);
}

void useFramebuffer(framebuffer* fb) {
    for(int i = 0; i < fb->numAttachments; i++) {
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, fb->colorAttachments[i]);
    }
}
