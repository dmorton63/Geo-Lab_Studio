#pragma once
#include <glad/glad.h>
#include <iostream>

class Framebuffer {
public:
    Framebuffer() : fbo_(0), colorTexture_(0), depthStencilRBO_(0), width_(0), height_(0) {}

    ~Framebuffer() {
        cleanup();
    }

    void init(int width, int height) {
        width_ = width;
        height_ = height;

        // Create framebuffer
        glGenFramebuffers(1, &fbo_);
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);

        // Create color texture
        glGenTextures(1, &colorTexture_);
        glBindTexture(GL_TEXTURE_2D, colorTexture_);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexture_, 0);

        // Create depth/stencil renderbuffer
        glGenRenderbuffers(1, &depthStencilRBO_);
        glBindRenderbuffer(GL_RENDERBUFFER, depthStencilRBO_);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, width, height);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, depthStencilRBO_);

        // Check framebuffer completeness
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "ERROR: Framebuffer is not complete!" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void bind() {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo_);
        glViewport(0, 0, width_, height_);
    }

    void unbind() {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void resize(int width, int height) {
        if (width == width_ && height == height_) return;
        
        cleanup();
        init(width, height);
    }

    void cleanup() {
        if (colorTexture_) {
            glDeleteTextures(1, &colorTexture_);
            colorTexture_ = 0;
        }
        if (depthStencilRBO_) {
            glDeleteRenderbuffers(1, &depthStencilRBO_);
            depthStencilRBO_ = 0;
        }
        if (fbo_) {
            glDeleteFramebuffers(1, &fbo_);
            fbo_ = 0;
        }
    }

    GLuint getColorTexture() const { return colorTexture_; }
    int getWidth() const { return width_; }
    int getHeight() const { return height_; }

private:
    GLuint fbo_;
    GLuint colorTexture_;
    GLuint depthStencilRBO_;
    int width_;
    int height_;
};
