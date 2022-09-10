#ifndef MODEL_3D
#define MODEL_3D


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
#include "Helpers.h"

struct Vertex {
    float x, y, z;
    float r, g, b;
    float xNorm, yNorm, zNorm;
    float xTex, yTex;
    Vertex(float x, float y, float z):x(x), y(y), z(z){}
    Vertex(float x, float y, float z, float r, float g, float b,
           float inXNorm, float inYNorm, float inZNorm, float inXTex, float inYTex);
    void clearNorm();
    glm::vec3 posVec();
};

struct Indice{
    int first;
    int second;
    int third;
    Indice(int in_first, int in_second, int in_third);
};

void loadOff(std::vector<Indice> &indices, std::vector<Vertex> &vertices, std::string file_name);
void calcNorm(std::vector<Indice> &indices, std::vector<Vertex> &vertices);

class OFFModel {
public:
    glm::mat4 model;
    GLuint VAO;
    GLuint VBO;
    GLuint EBO;
    //these cannot be used since EBO has a hard time working with multiple VBOs
    //So instead the structure is rebuilt using openGL tutorial's structure as
    //all required data in one VBO and assigning with correct order in vertexAttribPointer
    //later using layout to correct allocate to the shader.
    GLuint VBOColor;
    GLuint VBONorm;
    GLuint LVBOTex;
    std::vector<Vertex> vertices;
    float* verticesArr = nullptr;
    std::vector<Indice> indices;
    int* indicesArr = nullptr;
    bool hasEnv;

    OFFModel():VAO(0), VBO(0), EBO(0){model = glm::mat4(1.0f);}
    OFFModel(std::vector<Vertex> in_vert, std::vector<Indice> in_ind);

    void translate(float xTranslate, float yTranslate, float zTranslate);
    void translate(glm::vec3 translate);
    void scale(float scale_factor);
    void rotate(float angle, int given_axis);    
    glm::vec3 calc_bary();
    void render_model();
    void render_model(GLuint id);
    void normalize_loc();
    float* verttoArr(){
        int size_of_arr = 11 * vertices.size();
        float output[size_of_arr];
        int j = 0;
        for(int i = 0; i < size_of_arr; i += 11){
            output[i] = vertices[j].x;
            output[i + 1] = vertices[j].y;
            output[i + 2] = vertices[j].z;
            output[i + 3] = vertices[j].r;
            output[i + 4] = vertices[j].g;
            output[i + 5] = vertices[j].b;
            output[i + 6] = vertices[j].xNorm;
            output[i + 7] = vertices[j].yNorm;
            output[i + 8] = vertices[j].zNorm;
            output[i + 9] = vertices[j].xTex;
            output[i + 10] = vertices[j].yTex;

            j += 1;
        }
        return output;
    }
    int* indtoArr(){
        //std::cout << "indice size is: " << indices.size();
        int size_of_arr = 3 * indices.size();
        //std::cout << "size_of_arr is: " << size_of_arr << std::endl;
        int output [size_of_arr];
        int j = 0;
        for(int i = 0; i < size_of_arr; i += 3){
            output[i] = indices[j].first;
            output[i + 1] = indices[j].second;
            output[i + 2] = indices[j].third;
            j += 1;
        }
        return output;
    }
    std::vector<glm::vec3> vertoVec3(){
        std::vector<glm::vec3> output;
        for(int i = 0; i < vertices.size(); i++){
            glm::vec3 temp;
            temp.x = vertices[i].x;
            temp.y = vertices[i].y;
            temp.z = vertices[i].z;
            output.push_back(temp);
        }
        return output;
    };
    std::vector<glm::vec3> indtoVec3(){
        std::vector<glm::vec3> output;
        for(int i = 0; i < indices.size(); i++){
            glm::vec3 temp;
            temp.x = indices[i].first;
            temp.y = indices[i].second;
            temp.z = indices[i].third;
            output.push_back(temp);
        }
        return output;
    };
};

#endif