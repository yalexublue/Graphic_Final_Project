#include "OFFModel.h"
#include <algorithm>

OFFModel::OFFModel(std::vector<Vertex> in_vert, std::vector<Indice> in_ind) : OFFModel() {

    this->vertices = in_vert;
    this->indices = in_ind;
    //vertices data contains pos, norm

    //read in data check
    /*
    for (int i = 0; i < vertices.size(); i++){
        std::cout << vertices[i].x <<", " << vertices[i].y << ", " << vertices[i].z << std::endl;

    }
    for (int i = 0; i < vertices.size(); i++){
        std::cout << indices[i].first <<", " << indices[i].second << ", " << indices[i].third << std::endl;

    }
     */


    /*
    std::vector<glm::vec3> tempVer;
    std::vector<glm::vec3> tempColor;
    std::vector<glm::vec3> tempNorm;
    std::vector<glm::vec2> tempTex;
    for (int i = 0; i < vertices.size(); i++){
        tempVer.push_back(glm::vec3(vertices[i].x, vertices[i].y, vertices[i].z));
        tempColor.push_back(glm::vec3(vertices[i].r, vertices[i].g, vertices[i].b));
        tempNorm.push_back(glm::vec3(vertices[i].xNorm, vertices[i].yNorm, vertices[i].zNorm));
        tempTex.push_back(glm::vec2(vertices[i].xTex, vertices[i].yTex));
    }
     */
    normalize_loc();
    hasEnv = false;
    verticesArr = verttoArr();
    indicesArr = indtoArr();


    //bind its own VAO later to be referenced
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    //standard procedure for VBO generate and bind
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), verticesArr, GL_STATIC_DRAW);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    //glBindBuffer(GL_ARRAY_BUFFER, VBO);
    //glBufferData(GL_ARRAY_BUFFER, tempVer.size() * 3, vertices.data(), GL_STATIC_DRAW);

    //std::cout << "vertices size is: " << vertices.size() << std::endl;
    //std::cout << "Vertex size is: " << sizeof(Vertex) << std::endl;

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Indice), indicesArr, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(Indice), indices.data(), GL_STATIC_DRAW);
    //std::cout << "indices size is: " << indices.size() << std::endl;

    //glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void*)0);
    //glEnableVertexAttribArray(0);

    //position
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(0 * 3 * sizeof(float)));
    glEnableVertexAttribArray(0);
    //color
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(1 * 3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    //normal
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(2 * 3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    //tex
    glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(3 * 3 * sizeof(float)));
    glEnableVertexAttribArray(3);

    //clear
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

glm::vec3 OFFModel::calc_bary() {
    glm::vec3 bary(0.0f);

    for (Vertex vert : vertices) {
        bary += (glm::vec3(model * glm::vec4(vert.posVec(), 1)) / (float)vertices.size());
    }
    return bary;
}

void OFFModel::normalize_loc() {
    // find max dimension
    std::vector<float> xs, ys, zs;
    for (size_t i = 0; i < vertices.size(); i++) {
        Vertex curr_vert = vertices.at(i);
        if (i % 3 == 0) {
            xs.push_back(curr_vert.x);
        } else if (i % 3 == 1) {
            ys.push_back(curr_vert.y);
        } else {
            zs.push_back(curr_vert.z);
        }
    }
    float x_range = *std::max_element(xs.begin(), xs.end()) - *std::min_element(xs.begin(), xs.end());
    float y_range = *std::max_element(ys.begin(), ys.end()) - *std::min_element(ys.begin(), ys.end());
    float z_range = *std::max_element(zs.begin(), zs.end()) - *std::min_element(zs.begin(), zs.end());
    float max_dim = std::max(x_range, std::max(y_range, z_range));

    // move barycenter to origin and divide all vertices by max dimension
    glm::vec3 bary = calc_bary();
    for (Vertex& v : vertices) {
        v.x -= bary.x;
        v.x /= max_dim;
        v.y -= bary.y;
        v.y /= max_dim;
        v.z -= bary.z;
        v.z /= max_dim;
    }
}


void OFFModel::render_model() {
    //Repoint to each model's own VAO
    //std::cout << "VAO, VBO, IBO: " << VAO << ", " << VBO << ", " << EBO << std::endl;
    glBindVertexArray(VAO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glDrawElements(GL_TRIANGLES, 3 * indices.size(), GL_UNSIGNED_INT, 0);
    /*
    int32_t bsize = 0;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &bsize);
    if (bsize == 0){
        std::cout << "VBO is empty\n";
    }
    else
        std::cout << "VBO is not empty\n";

    int32_t esize = 0;
    glGetBufferParameteriv(GL_ELEMENT_ARRAY_BUFFER, GL_BUFFER_SIZE, &esize);
    if (bsize == 0){
        std::cout << "VBO is empty\n";
    }
    else
        std::cout << "VBO is not empty\n";
    */
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void OFFModel::render_model(GLuint id) {

    //Repoint to each model's own VAO
    //std::cout << "VAO, VBO, IBO: " << VAO << ", " << VBO << ", " << EBO << std::endl;
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    glVertexAttribPointer(id, 3, GL_FLOAT, GL_FALSE, 11 * sizeof(float), (void *)(3 * id * sizeof(float)));
    //glVertexAttribPointer(id, VBO.rows, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(id);

    glDrawArrays(GL_TRIANGLES, 0, 3 * vertices.size());
    //glDrawElements(GL_TRIANGLES, 3 * indices.size(), GL_UNSIGNED_INT, 0);

    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

}

void OFFModel::translate(glm::vec3 translate) {
    model = glm::translate(glm::mat4(1), translate) * model;
}

void OFFModel::translate(float xTranslate, float yTranslate, float zTranslate) {
    translate(glm::vec3(xTranslate, yTranslate, zTranslate));
}

void OFFModel::scale(float scale_factor) {
    glm::mat4 trans(1), scale(1);
    trans = glm::translate(trans, -calc_bary());
    scale = glm::scale(scale, glm::vec3(scale_factor));
    model = glm::inverse(trans) * scale * trans * model;
}

void OFFModel::rotate(float angle, int given_axis) {

    glm::vec3 axis(0);
    axis[given_axis] = 1.0f;
    glm::mat4 trans(1), rotate(1);
    trans = glm::translate(trans, -calc_bary());
    rotate = glm::rotate(rotate, glm::radians(angle), axis);
    model = glm::inverse(trans) * rotate * trans * model;
}


Vertex::Vertex(float x, float y, float z, float r, float g, float b,
               float inXNorm, float inYNorm, float inZNorm, float inXTex, float inYTex)
        : x(x), y(y), z(z), r(r), g(g), b(b), xNorm(inXNorm), yNorm(inYNorm), zNorm(inZNorm), xTex(inXTex), yTex(inYTex)
{}

Indice::Indice(int in_first, int in_second, int in_third): first(in_first), second(in_second), third(in_third) {}

void Vertex::clearNorm() {
    xNorm = 0;
    yNorm = 0;
    zNorm = 0;
}

glm::vec3 Vertex::posVec() {
    return glm::vec3(x, y, z);
}

void loadOff(std::vector<Indice> &indices, std::vector<Vertex> &vertices, std::string file_name) {
    std::ifstream file(file_name);
    if (!file) {
        std::cout << "file loading fails: " << file_name << std::endl;
    }

    std::string header;
    int num_vertices, num_faces, _;
    file >> header >> num_vertices >> num_faces >> _;

    for (int i = 0; i < num_vertices; i++) {
        float x, y, z;
        file >> x >> y >> z;

        switch (i % 3) {
            case 0:
                vertices.push_back(Vertex(x, y, z, 0, 0, 1, 0, 0, 0, 0, 0));
                break;
            case 1:
                vertices.push_back(Vertex(x, y, z, 0, 0, 1, 0, 0, 0, 1, 0));
                break;
            case 2:
                vertices.push_back(Vertex(x, y, z, 0, 0, 1, 0, 0, 0, 0.5, 1));
                break;
        }
    }

    for (int i = 0; i < num_faces; i++) {
        int edge, i1, i2, i3;
        file >> edge >> i1 >> i2 >> i3;
        indices.push_back(Indice(i1, i2, i3));
    }
    file.close();
}

void calcNorm(std::vector<Indice> &indices, std::vector<Vertex> &vertices) {
    for(Vertex& v : vertices) {
        v.clearNorm();
    }

    for(Indice& tri : indices) {
        int i1 = tri.first, i2 = tri.second, i3 = tri.third;
        glm::vec3 v1 = vertices[tri.second].posVec() - vertices[tri.first].posVec();
        glm::vec3 v2 = vertices[tri.third].posVec() - vertices[tri.first].posVec();
        glm::vec3 norm = glm::normalize(glm::cross(v1, v2));

        vertices[i1].xNorm += norm.x;
        vertices[i2].xNorm += norm.x;
        vertices[i3].xNorm += norm.x;
        vertices[i1].yNorm += norm.y;
        vertices[i2].yNorm += norm.y;
        vertices[i3].yNorm += norm.y;
        vertices[i1].zNorm += norm.z;
        vertices[i2].zNorm += norm.z;
        vertices[i3].zNorm += norm.z;
    }
}
