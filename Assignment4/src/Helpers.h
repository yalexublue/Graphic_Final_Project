#ifndef HELPERS
#define HELPERS

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#define GLM_FORCE_SWIZZLE
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>



#ifdef _WIN32
#  include <windows.h>
#  undef max
#  undef min
#  undef DrawText
#endif

#ifndef __APPLE__
#  define GLEW_STATIC
#  include <GL/glew.h>
#endif

#ifdef __APPLE__
#   include <OpenGL/gl3.h>
#   define __gl_h_ /* Prevent inclusion of the old gl.h */
#else
#   ifdef _WIN32
#       include <windows.h>
#   endif
#   include <GL/gl.h>
#endif

#include <GLFW/glfw3.h>


// From: https://blog.nobel-joergensen.com/2013/01/29/debugging-opengl-using-glgeterror/
void _check_gl_error(const char *file, int line);

///
/// Usage
/// [... some opengl calls]
/// glCheckError();
///
#define check_gl_error() _check_gl_error(__FILE__,__LINE__)


class Camera {
public:
    float size;
    float x;
    float y;
    glm::vec3 position;
    glm::vec3 lookup;

    Camera(){}
    Camera(glm::vec3 start_pos, glm::vec3 start_up);
    glm::mat4 getLookAt(){return glm::lookAt(position, glm::vec3(0), lookup);}
    void update();

};

class Shadow {
public:
    GLuint shadowWidth;
    GLuint shadowHeight;
    GLuint shadowMap;
    GLuint FBO;
    Shadow(): FBO(0), shadowMap(0){}
    bool init(GLuint width, GLuint height);
    void bindFramebuffer(){glBindFramebuffer(GL_FRAMEBUFFER, FBO);}
    void useTexture(GLenum texture){
        glActiveTexture(texture);
        glBindTexture(GL_TEXTURE_2D, shadowMap);
    }

};

class Light {
public:
    glm::vec3 direction;
    glm::vec3 color;
    float ambient;
    float diffuse;
    glm::mat4 light;
    Shadow* shadow;
    Light();
    Light(float shadowWidth, float shadowHeight,
          float r, float g, float b,
          float ambient, float diffuse,
          float dir_x, float dir_y, float dir_z);

    void activate(float lightColor, float inAmbient,
                  float inDiffuse, float inDirection);
    glm::mat4 getMatrix(){return light * glm::lookAt
                        (-direction, glm::vec3(0), glm::vec3(0, 1, 0));}

};

//Redefined shader program class for easier shader control
class Program {
public:
    GLuint ID;
    GLuint model;
    GLuint light;
    GLuint shadow;
    GLuint tex;

    Program(): ID(0), model(0){}
    Program(const std::string &vertex_code, const std::string &fragment_code);

    void uniformTex(GLuint texture){glUniform1i(tex, texture); }
    void uniformShadow(GLuint texture){glUniform1i(shadow, texture); }
    void uniformLight(glm::mat4 inLight){
        glUniformMatrix4fv(light, 1, GL_FALSE, glm::value_ptr(inLight));
    }
    void set_mat4(const std::string &name, const glm::mat4 &mat) const{
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    //report attrib id based on attrib name, port over from old program class.
    GLint attrib_loc(const std::string &name) const{ return glGetAttribLocation(ID, name.c_str());}
    void validate();
    void use(){glUseProgram(ID);}
    void compile();
};

#endif