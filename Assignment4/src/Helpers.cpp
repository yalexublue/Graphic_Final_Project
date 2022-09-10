#include "Helpers.h"
#include <math.h>
#include <iostream>
#include <fstream>

Camera::Camera(glm::vec3 start_pos, glm::vec3 start_up) : position(start_pos), lookup(start_up) {
    size = glm::length(position);
    x = acos(position[1] / size);
    y = atan(position[0] / position[2]);
}

void Camera::update() {
    position.x = size * sin(y) * sin(x);
    position.y = size * cos(x);
    position.z = size * cos(y) * sin(x);
}

Light::Light() {
    color = glm::vec3(1, 1, 1);
    ambient = 1.0f;
    diffuse = 0.0f;
    direction = glm::vec3(0, -1, 0);
}

bool Shadow::init(GLuint width, GLuint height) {
    shadowWidth = width;
    shadowHeight = height;

    glGenFramebuffers(1, &FBO);
    glGenTextures(1, &shadowMap);
    glBindTexture(GL_TEXTURE_2D, shadowMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowWidth, shadowHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowMap, 0);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (status != GL_FRAMEBUFFER_COMPLETE) {
        printf("Framebuffer error: %i\n", status);
        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    return true;
}

Light::Light(float shadowWidth, float shadowHeight,
             float r, float g, float b,
             float ambient, float diffuse,
             float dir_x, float dir_y, float dir_z) {
    shadow = new Shadow();
    shadow->init(shadowWidth, shadowHeight);

    color = glm::vec3(r, g, b);
    this->diffuse = diffuse;
    this->ambient = ambient;

    direction = glm::vec3(dir_x, dir_y, dir_z);
    light = glm::ortho(-50.0f, 50.0f, -50.0f, 50.0f, 0.1f, 100.0f);
}

void Light::activate(float lightColor, float inAmbient,
                     float inDiffuse, float inDirection) {
    glUniform3f(lightColor, color.x, color.y, color.z);
    glUniform1f(inAmbient, ambient);
    glUniform1f(inDiffuse, diffuse);
    glUniform3f(inDirection, direction.x, direction.y, direction.z);
}

Program::Program(const std::string &vertex_code, const std::string &fragment_code) {
    ID = glCreateProgram();

    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    const char *vertex_string_const = vertex_code.c_str();
    glShaderSource(vertex_shader, 1, &vertex_string_const, NULL);
    glCompileShader(vertex_shader);
    glAttachShader(ID, vertex_shader);
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    const char *fragment_string_const = fragment_code.c_str();
    glShaderSource(fragment_shader, 1, &fragment_string_const, NULL);
    glCompileShader(fragment_shader);
    glAttachShader(ID, fragment_shader);

    compile();
}

void Program::validate() {
    GLint result = 0;
    GLchar buffer[512] = { 0 };

    glValidateProgram(ID);
    glGetProgramiv(ID, GL_VALIDATE_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(ID, 512, NULL, buffer);
        std::cout << "Error shader id is: " << ID << std::endl;
        return;
    }
}

void Program::compile() {
    glLinkProgram(ID);
    model = glGetUniformLocation(ID, "model");
    tex = glGetUniformLocation(ID, "tex");
    light = glGetUniformLocation(ID, "light_transform");
    shadow = glGetUniformLocation(ID, "shadowMap");
}