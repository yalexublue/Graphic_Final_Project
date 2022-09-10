//TODO: w,a,s,d camera control, lookup, down, right, left object translation
//TODO: R toggle red shadow, E toggle environment mapping, PERIOD remove item
//TODO: X, C, V, B, N, M rotation based on all axis.

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <math.h>
#include <iostream>
#include "Helpers.h"
#include "OFFModel.h"
#include "off.h"




int winWidth;
int winHeight;
float cubeVertData[] = {
        0.5f, 0.5f, 0.5f,
        -0.5f, 0.5f, 0.5f,
        -0.5f, -0.5f, 0.5f,
        0.5f, -0.5f, 0.5f,
        0.5f, 0.5f, -0.5f,
        -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, -0.5f,
        0.5f, -0.5f, -0.5f
};
unsigned int cubeIndData[] = {
        1, 3, 0,
        2, 3, 1,
        7, 5, 4,
        7, 6, 5,
        0, 4, 5,
        5, 1, 0,
        6, 3, 2,
        6, 7, 3,
        2, 1, 5,
        6, 2, 5,
        3, 4, 0,
        3, 7, 4
};
float cubemapVert[] = {
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
        1.0f,  1.0f, -1.0f,
        1.0f,  1.0f,  1.0f,
        1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
        1.0f, -1.0f,  1.0f
};
//two triangle formed square planeData
float planeData[] = {
        3.0f, -1.05f, 3.0f, //first vertex pos
        1.0f, 1.0f, 1.0f,  //first vertex color
        0.0f, 1.0f, 0.0f,  //first vertex norm

        -3.0f, -1.05f, 3.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,

        -3.0f, -1.05f, -3.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,

        3.0f, -1.05f,  3.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,

        -3.0f, -1.05f, -3.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f,

        3.0f, -1.05f, -3.0f,
        1.0f, 1.0f, 1.0f,
        0.0f, 1.0f, 0.0f
};
int selectedObj = -1;
bool hasRedShadow = false;
bool hadEnvMapping = false;
bool mouseHolding = false;
float refraction = 0.5;
float rotation = 0;
float pi = M_PI;

glm::mat4 proj(1.0f);
glm::mat4 view(1.0f);
glm::mat4 model(1.0f);

GLuint VAOPlane;
GLuint VBOPlane;
GLuint VAOCubemap;
GLuint VBOCubemap;
GLuint position = 0;
GLuint norms = 0;
GLuint uniformColor = 0;
GLuint tex = 0;
GLuint uniformProj = 0;
GLuint uniformModel = 0;
GLuint uniformView = 0;
GLuint uniformCamPos = 0;
GLuint uniformShine = 0;
GLuint uniformSpecular = 0;
GLuint uniformLightColor = 0;
GLuint uniformLightDirection = 0;
GLuint uniformAmbient = 0;
GLuint uniformDiffuse = 0;
GLuint uniformRedShadow = 0;
GLuint uniformEnvMapping = 0;
GLuint uniformRefract = 0;

std::vector<Indice> cubeInd;
std::vector<Vertex> cubeVert;
std::vector<Indice> rabbitInd;
std::vector<Vertex> rabbitVert;
std::vector<Indice> bumpyInd;
std::vector<Vertex> bumpyVert;
std::vector<glm::vec3> normDataSeparate;
std::vector<OFFModel*> models;
Camera camera;
Light light;

void key_callback(GLFWwindow* window, int key, int code, int action, int mode);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

//updown result maybe differ with implementation of stbi flip
std::vector<std::string> cubemap_faces{
    //right and left
    "../data/night/night_posx.png",
    "../data/night/night_negx.png",
    //up and down
    "../data/night/night_posy.png",
    "../data/night/night_negy.png",
    //front and back
    "../data/night/night_posz.png",
    "../data/night/night_negz.png"
};

//cubemap data loading, based on learnOpenGL using external stbi library
GLuint loadCubemap(std::vector<std::string> faces) {
    //flip the sky, since original is upside down somehow.
    stbi_set_flip_vertically_on_load(true);
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data) {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else {
            std::cout << "loading fails" << std::endl;
            stbi_image_free(data);
        }

    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}

void key_callback(GLFWwindow* window, int key, int code, int action, int mode) {
    switch (key) {
        case GLFW_KEY_1:
            if (action == GLFW_PRESS) {
                OFFModel *mesh = new OFFModel(cubeVert, cubeInd);
                models.push_back(mesh);
            }
            break;
        case GLFW_KEY_2:
            if (action == GLFW_PRESS) {
                OFFModel *mesh = new OFFModel(bumpyVert, bumpyInd);
                models.push_back(mesh);
            }
            break;
        case GLFW_KEY_3:
            if (action == GLFW_PRESS) {
                OFFModel *mesh = new OFFModel(rabbitVert, rabbitInd);
                models.push_back(mesh);
            }
            break;
        case GLFW_KEY_RIGHT:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->translate(0.2f, 0.0f, 0.0f);
            }
            break;
        case GLFW_KEY_LEFT:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->translate(-0.2f, 0.0f, 0.0f);
            }
            break;
        case GLFW_KEY_UP:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                    models[selectedObj]->translate(0.0f, 0.2f, 0.0f);
            }
            break;
        case GLFW_KEY_DOWN:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->translate(0.0f, -0.2f, 0.0f);
            }
            break;
        case GLFW_KEY_MINUS:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->translate(0.0f, 0.0f, 0.2f);
            }
            break;
        case GLFW_KEY_EQUAL:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->translate(0.0f, 0.0f, -0.2f);
            }
            break;
        case GLFW_KEY_J:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->scale(1.1);
            }
            break;
        case GLFW_KEY_K:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->scale(0.9);
            }
            break;
        case GLFW_KEY_V:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->rotate(30, 0);
            }
            break;
        case GLFW_KEY_B:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->rotate(-30, 0);
            }
            break;
        case GLFW_KEY_N:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->rotate(30, 1);
            }
            break;
        case GLFW_KEY_M:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->rotate(-30, 1);
            }
            break;
        case GLFW_KEY_X:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->rotate(30, 2);
            }
            break;
        case GLFW_KEY_C:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->rotate(-30, 2);
            }
            break;
        case GLFW_KEY_W:
            if (action == GLFW_PRESS) {
                camera.x = std::max(camera.x - 0.2, 0.01);
                camera.update();
            }
            break;
        case GLFW_KEY_S:
            if (action == GLFW_PRESS) {
                camera.x = std::max(camera.x + 0.2, 0.01);
                camera.update();
            }
            break;
        case GLFW_KEY_A:
            if (action == GLFW_PRESS) {
                camera.y -= 0.2;
                camera.update();
            }
            break;
        case GLFW_KEY_D:
            if (action == GLFW_PRESS) {
                camera.y += 0.2;
                camera.update();
            }
            break;
        case GLFW_KEY_O:
            if (action == GLFW_PRESS) {
                camera.size += 0.2;
                camera.update();
            }
            break;
        case GLFW_KEY_P:
            if (action == GLFW_PRESS) {
                camera.size -= 0.2;
                camera.update();
            }
            break;
        case GLFW_KEY_PERIOD:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models.erase(models.begin() + selectedObj);
                selectedObj = -1;
            }
            break;
        case GLFW_KEY_R:
            if (action == GLFW_PRESS) {
                hasRedShadow = !hasRedShadow;
            }
            break;
        case GLFW_KEY_E:
            if (action == GLFW_PRESS && selectedObj >= 0) {
                models[selectedObj]->hasEnv = !models[selectedObj]->hasEnv;
            }
            break;
    }
}

bool inObj(GLFWwindow* window, OFFModel object) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);
    int width, height;
    glfwGetWindowSize(window, &width, &height);

    float xmin = width;
    float xmax = 0;
    float ymin = height;
    float ymax = 0;
    // Convert screen position to world coordinates
    double xworld = ((xpos/double(width))*2)-1;
    double yworld = (((height-1-ypos)/double(height))*2)-1; // NOTE: y axis is flipped in glfw

    for (int i = 0; i < 2; i++) {
        for (int j = 0; j < 2; j++) {
            for (int m = 0; m < 2; m++) {
                glm::vec4 world_coords(i - 0.5, j - 0.5, m - 0.5, 1.0);
                //calculate the location based on model, projection and view change
                glm::vec4 canonical_coords = proj * view * (object.model) * world_coords;
                float x = canonical_coords[0] / canonical_coords[3];
                float y = canonical_coords[1] / canonical_coords[3];
                x = (x / 2 + 0.5f) * width;
                y = (y / 2 + 0.5f) * height;
                y = height - 1 - y;
                xmin = std::min(xmin, x);
                ymin = std::min(ymin, y);
                xmax = std::max(xmax, x);
                ymax = std::max(ymax, y);
            }
        }
    }
    bool x_in_range = xpos > xmin && xpos < xmax;
    bool y_in_range = ypos > ymin && ypos < ymax;
    return (x_in_range && y_in_range);
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    double xpos, ypos;
    glfwGetCursorPos(window, &xpos, &ypos);

    int width, height;
    glfwGetWindowSize(window, &width, &height);

    if(button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mouseHolding = true;
        //std::cout << "mouse holding or not: " << mouseHolding << std::endl;
        selectedObj = -1;
        for (size_t i = 0; i < models.size(); i++) {
            if (inObj(window, *(models.at(i)))) {
                selectedObj = i;
                return;
            }
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE){
        mouseHolding = false;
        //std::cout << "mouse holding or not: " << mouseHolding << std::endl;
    }
    //std::cout << "selected model index is: " << selectedObj << std::endl;
}

int main() {
    GLFWwindow* window;

    // Initialize the library
    if (!glfwInit())
        return -1;

    // Activate supersampling
    glfwWindowHint(GLFW_SAMPLES, 8);

    // Ensure that we get at least a 3.2 context
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

    // On apple we have to load a core profile with forward compatibility
    #ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

    // Create a windowed mode window and its OpenGL context
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);

    #ifndef __APPLE__
        glewExperimental = true;
        GLenum err = glewInit();
        if (GLEW_OK != err) {
            fprintf(stderr, "Error: %s\n", glewGetErrorString(err)); /* Problem: glewInit failed, something is seriously wrong. */
        }
        glGetError(); // pull and savely ignore unhandled errors like GL_INVALID_ENUM
        fprintf(stdout, "Status: Using GLEW %s\n", glewGetString(GLEW_VERSION));
    #endif

    int major, minor, rev;
    major = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MAJOR);
    minor = glfwGetWindowAttrib(window, GLFW_CONTEXT_VERSION_MINOR);
    rev = glfwGetWindowAttrib(window, GLFW_CONTEXT_REVISION);
    printf("OpenGL version recieved: %d.%d.%d\n", major, minor, rev);
    printf("Supported OpenGL is %s\n", (const char*)glGetString(GL_VERSION));
    printf("Supported GLSL is %s\n", (const char*)glGetString(GL_SHADING_LANGUAGE_VERSION));

    //register mouse and keyboard
    glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);
    glEnable(GL_DEPTH_TEST);

    glfwGetWindowSize(window, &winWidth, &winHeight);
    glViewport(0, 0, winWidth, winHeight);

    const std::string vertexShader = R"glsl(
        #version 330

        //assign layouts for accurate attribute array assignment
        layout (location = 0) in vec3 pos;
        layout (location = 1) in vec3 color;
        layout (location = 2) in vec3 norm;
        layout (location = 3) in vec2 tex;
        out vec4 vColor;
        out vec3 normal;
        out vec3 FragPos;
        out vec2 texCoord;
        out vec4 lightPos;
        uniform mat4 model;
        uniform mat4 proj;
        uniform mat4 view;
        uniform mat4 light_transform;

        void main() {
            gl_Position = proj * view * model * vec4(pos, 1.0);
            lightPos = light_transform * model * vec4(pos, 1.0);
            vColor = vec4(color, 1.0f);
            normal = mat3(transpose(inverse(model))) * norm;
            FragPos = (model * vec4(pos, 1.0)).xyz;
            texCoord = tex;
        })glsl";

    const std::string fragShader = R"glsl(
        #version 330

        in vec4 vColor;
        in vec3 normal;
        in vec3 FragPos;
        in vec2 texCoord;
        in vec4 lightPos;
        out vec4 outColor;
        uniform vec3 color;
        uniform bool hasRedShadow;
        uniform bool hasEnv;
        uniform float refraction;
        struct Light{
            vec3 color;
            float ambient;
            vec3 direction;
            float diffuse;
        };
        uniform vec3 camera_pos;
        uniform Light light;
        uniform float specular;
        uniform float shininess;
        uniform sampler2D tex;
        uniform sampler2D shadowMap;
        uniform samplerCube cubemapVert;

        float ShadowCalculation(Light light) {
            vec3 projCoords = lightPos.xyz / lightPos.w;
            projCoords = (projCoords * 0.5) + 0.5;
            vec3 normal = normalize(normal);
            vec3 lightDirection = normalize(light.direction);
            float depth = projCoords.z;
            float bias = max(0.01 * (1.0 - dot(normal, lightDirection)), 0.005);
            float shadow = 0.0;
            vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
            for(int x = -1; x <= 1; ++x) {
                for(int y = -1; y <= 1; ++y) {
                    float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r;
                    shadow += depth - bias > pcfDepth ? 1.0 : 0.0;
                }
            }
            shadow /= 9.0;
            if (projCoords.z > 1.0) {
                shadow = 0.0f;
            }
            return shadow;
        }

        vec4 find_light_color() {
            float shadowFactor = ShadowCalculation(light);
            if (hasRedShadow && shadowFactor > 0.0005) {
                return vec4(1, 0, 0, 1);
            }

            vec3 direction = normalize(-light.direction);
            vec4 ambientColor = vec4(light.color, 1.0f) * light.ambient;
            float diffuseFactor = max(dot(normalize(normal), normalize(direction)), 0.0f);
            vec4 diffuseColor = vec4(light.color * light.diffuse * diffuseFactor, 1.0f);
            vec4 specularColor = vec4(0, 0, 0, 0);

            if (diffuseFactor > 0.0f) {
                vec3 cameraToFrag = normalize(camera_pos - FragPos);
                vec3 reflectedVertex = normalize(reflect(-direction, normalize(normal)));
                float specularFactor = max(dot(cameraToFrag, reflectedVertex), 0.0f);
                specularFactor = pow(specularFactor, shininess);
                specularColor = vec4(light.color * specular * specularFactor, 1.0f);
            }
            return (ambientColor + (1 - shadowFactor) * (diffuseColor + specularColor));
        }

        void main() {
            vec3 I = normalize(FragPos - camera_pos);
            //here refraction is implemented instead of reflection according to tutorial
            vec3 R = refract(I, normalize(normal), refraction);
            vec4 FragColor = find_light_color();
            //used to toggle environment mapping on and off
            if (hasEnv) {
                outColor = vec4(texture(cubemapVert, R).rgb, 1.0);
            } else {
                outColor = vColor * vec4(color, 1.0f) * FragColor;
            }
    })glsl";

    const std::string shadowVertexShader = R"glsl(
        #version 330

        in vec3 pos;
        uniform mat4 model;
        uniform mat4 light_transform;
        void main() {
            gl_Position = light_transform * model * vec4(pos, 1.0);
        })glsl";
    //This is meant to be left empty
    const std::string shadowFragmentShader = R"glsl(
        #version 330 core

        void main()
        {
            // gl_FragDepth = gl_FragCoord.z;
        })glsl";

    const std::string cubemapVertexShader = R"glsl(
        #version 330 core

        in vec3 aPos;
        out vec3 TexCoords;
        uniform mat4 proj;
        uniform mat4 view;
        void main() {
            TexCoords = aPos;
            vec4 pos = proj * view * vec4(aPos, 1.0);
            gl_Position = pos.xyww;
        })glsl";

    const std::string cubemapFragmentShader = R"glsl(
        #version 330 core

        out vec4 FragColor;
        in vec3 TexCoords;
        uniform samplerCube cubemapVert;
        void main() {
            FragColor = texture(cubemapVert, TexCoords);
        })glsl";


    loadOff(rabbitInd, rabbitVert, "../data/bunny.off");
    loadOff(bumpyInd, bumpyVert, "../data/bumpy_cube.off");
    //add cube data
    int vCount = 0;
    for (int i = 0; i < 8; i++){
        float x = cubeVertData[vCount];
        float y = cubeVertData[vCount + 1];
        float z = cubeVertData[vCount + 2];
        //std::cout << x << ", " << y << ", " << z << std::endl;
        switch (i % 3) {
            case 0:
                cubeVert.push_back(Vertex(x, y, z, 1, 1, 1, 0, 0, 0, 0, 0));
                break;
            case 1:
                cubeVert.push_back(Vertex(x, y, z, 1, 1, 1, 0, 0, 0, 1, 0));
                break;
            case 2:
                cubeVert.push_back(Vertex(x, y, z, 1, 1, 1, 0, 0, 0, 0.5, 1));
                break;
        }
        vCount += 3;
    }

    int iCount = 0;
    for (int i = 0; i < 12; i++){
        int i1 = cubeIndData[iCount];
        int i2 = cubeIndData[iCount + 1];
        int i3 = cubeIndData[iCount + 2];
        cubeInd.push_back(Indice(i1, i2, i3));
        iCount += 3;
    }
    //vert norm calculation
    calcNorm(rabbitInd, rabbitVert);
    calcNorm(bumpyInd, bumpyVert);
    calcNorm(cubeInd, cubeVert);
    camera = Camera(glm::vec3(-2, 1, 7), glm::vec3(0, 1, 0));
    //light class object, a light generated with disignated parameters
    light = Light(10000, 10000, 1.0f, 1.0f, 1.0f, 0.2f, 0.5f, 3.0f, -1.0f, -3.0f);
    proj = glm::perspective(glm::radians(45.0f), (float)winWidth / winHeight, 0.1f, 100.0f);
    Program program(vertexShader, fragShader);
    Program shadow_program(shadowVertexShader, shadowFragmentShader);
    Program cubemap_program(cubemapVertexShader, cubemapFragmentShader);
    //bind cubemapVert data
    glGenVertexArrays(1, &VAOCubemap);
    glGenBuffers(1, &VBOCubemap);
    //std::cout << "cubemapVert VAO, VBO: " << VAOCubemap << ", " << VBOCubemap << std::endl;
    glBindVertexArray(VAOCubemap);
    glBindBuffer(GL_ARRAY_BUFFER, VBOCubemap);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cubemapVert), &cubemapVert, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    GLuint cubemap_tex = loadCubemap(cubemap_faces);
    cubemap_program.use();
    cubemap_program.uniformTex(cubemap_tex);
    glBindVertexArray(0);

    //bind planeData data
    glGenVertexArrays(1, &VAOPlane);
    glGenBuffers(1, &VBOPlane);
    glBindVertexArray(VAOPlane);
    glBindBuffer(GL_ARRAY_BUFFER, VBOPlane);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeData), &planeData, GL_STATIC_DRAW);

    //bind data to three locations, pos, color and norm, as layout designated in shader.
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(0 * 3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(1 * 3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 9, (void*)(2 * 3 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);

    // Loop until window closed
    while (!glfwWindowShouldClose(window)) {      
        glfwGetWindowSize(window, &winWidth, &winHeight);
        glViewport(0, 0, winWidth, winHeight);
        proj = glm::perspective(glm::radians(45.0f), (float)winWidth / winHeight, 0.1f, 100.0f);
        view = camera.getLookAt();

        // Clear window
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shadow_program.use();
        uniformModel = shadow_program.model;
        shadow_program.uniformLight(light.getMatrix());

        glViewport(0, 0, light.shadow->shadowWidth, light.shadow->shadowHeight);
        light.shadow->bindFramebuffer();
        glClear(GL_DEPTH_BUFFER_BIT);

        glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(uniformEnvMapping, GL_FALSE);

        glBindVertexArray(VAOPlane);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        glBindVertexArray(0);

        //drawing models, for red shadow

        for(size_t i = 0; i < models.size(); i++) {
            if (models[i]->hasEnv)
                glUniform1i(uniformEnvMapping, GL_TRUE);
            else
                glUniform1i(uniformEnvMapping, GL_FALSE);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(models[i]->model));
            if (selectedObj == i) {
                //std::cout << "mouse is holding" << std::endl;
                glUniform3f(uniformColor, 1.0f, 0.0f, 0.0f);
                models[i]->render_model();
                glUniform1i(uniformEnvMapping, GL_FALSE);
            }
            else {
                models[i]->render_model();
                glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
            }
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, winWidth * 2, winHeight * 2);
        glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        GLuint shader_ID = program.ID;
        uniformModel = glGetUniformLocation(shader_ID, "model");
        uniformProj = glGetUniformLocation(shader_ID, "proj");
        uniformView = glGetUniformLocation(shader_ID, "view");
        uniformColor = glGetUniformLocation(shader_ID, "color");
        uniformLightColor = glGetUniformLocation(shader_ID, "light.color");
        uniformLightDirection = glGetUniformLocation(shader_ID, "light.direction");
        uniformAmbient = glGetUniformLocation(shader_ID, "light.ambient");
        uniformDiffuse = glGetUniformLocation(shader_ID, "light.diffuse");
        uniformCamPos = glGetUniformLocation(shader_ID, "camera_pos");
        uniformSpecular = glGetUniformLocation(shader_ID, "specular");
        uniformShine = glGetUniformLocation(shader_ID, "shininess");
        uniformRedShadow = glGetUniformLocation(shader_ID, "hasRedShadow");
        uniformEnvMapping = glGetUniformLocation(shader_ID, "hasEnv");
        uniformRefract = glGetUniformLocation(shader_ID, "refraction");
        program.use();

        glUniformMatrix4fv(uniformProj, 1, GL_FALSE, glm::value_ptr(proj));
        glUniformMatrix4fv(uniformView, 1, GL_FALSE, glm::value_ptr(camera.getLookAt()));
        glUniform3f(uniformCamPos, camera.position.x, camera.position.y, camera.position.z);
        if (hasRedShadow)
            glUniform1i(uniformRedShadow, GL_TRUE);
        else
            glUniform1i(uniformRedShadow, GL_FALSE);

        if (hadEnvMapping)
            glUniform1i(uniformEnvMapping, GL_TRUE);
        else
            glUniform1i(uniformEnvMapping, GL_FALSE);
        glUniform1f(uniformRefract, refraction);
        light.direction.x = 2 * glm::sin(rotation);
        light.direction.z = 2 * glm::cos(rotation);
        rotation += 0.0025;
        light.activate(uniformLightColor, uniformAmbient, uniformDiffuse, uniformLightDirection);
        glUniform1f(uniformSpecular, 0.9f);
        glUniform1f(uniformShine, 120);

        program.uniformTex(1);
        program.uniformShadow(2);
        program.uniformLight(light.getMatrix());
        light.shadow->useTexture(GL_TEXTURE2);

        //model redraw with mouse toggle activated
        for(size_t i = 0; i < models.size(); i++) {
            if (models[i]->hasEnv)
                glUniform1i(uniformEnvMapping, GL_TRUE);
            else
                glUniform1i(uniformEnvMapping, GL_FALSE);
            glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(models[i]->model));
            if (selectedObj == i && mouseHolding) {
                glUniform1i(uniformEnvMapping, GL_FALSE);
                glUniform3f(uniformColor, 1.0f, 0.0f, 0.0f);
                models[i]->render_model();
            }
            else if (selectedObj == i && !mouseHolding) {

                if (models[selectedObj]->hasEnv) {
                    glUniform1i(uniformEnvMapping, GL_TRUE);
                    models[i]->render_model();
                    glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
                } else {
                    glUniform1i(uniformEnvMapping, GL_FALSE);
                    models[i]->render_model();
                    glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
                }
            }

            else {
                glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
                models[i]->render_model();
            }
        }

        glUniform3f(uniformColor, 1.0f, 1.0f, 1.0f);
        glUniformMatrix4fv(uniformModel, 1, GL_FALSE, glm::value_ptr(model));
        glUniform1i(uniformEnvMapping, GL_FALSE);

        glBindVertexArray(VAOPlane);
        glBindBuffer(GL_ARRAY_BUFFER, VBOPlane);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glDepthFunc(GL_LEQUAL);
        cubemap_program.use();
        cubemap_program.set_mat4("view", glm::mat4(glm::mat3(view)));
        cubemap_program.set_mat4("proj", proj);
        glBindVertexArray(VAOCubemap);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemap_tex);
        glDrawArrays(GL_TRIANGLES, 0, 36);


        glfwSwapBuffers(window);
        glfwPollEvents();

        //clear buffer and unbind
        glDepthFunc(GL_LESS);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
        glUseProgram(0);
    }

    glfwTerminate();

    return 0;
}



