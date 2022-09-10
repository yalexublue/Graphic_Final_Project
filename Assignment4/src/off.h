
#ifndef OFF_H
#define OFF_H

#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include "OFFModel.h"


// PPM file structure type
struct OFF {
    std::vector<Vertex> mVertex;
    std::vector<Indice> mIndice;
    unsigned int nVertex;
    unsigned int nFace;
    unsigned int nEdge;
    char mType[3];
    std::vector<glm::vec3> vertexToVec(){
        std::vector<glm::vec3> output;
        for (int i = 0; i < nVertex; i++){
            glm::vec3 temp;
            temp.x = mVertex[i].x;
            temp.y = mVertex[i].y;
            temp.z = mVertex[i].z;
            output.push_back(temp);
        }
        return output;

    }
    std::vector<glm::vec3> faceToVec(){
        std::vector<glm::vec3> output;
        for (int i = 0; i < nFace; i++){
            glm::vec3 temp;
            temp.x = mIndice[i].first;
            temp.y = mIndice[i].second;
            temp.z = mIndice[i].third;
            output.push_back(temp);
        }
        return output;
    }
};

// Reads a OFF file from the disk.
OFF * loadPPM(const std::string & fileName) {
    std::ifstream imageFile;
    imageFile.open(fileName, std::fstream::in);

    if (!imageFile) {
        std::cout << "Error opening image file " << fileName << std::endl;
        return nullptr;
    }

    OFF * ppmImage = new OFF;

    imageFile >> ppmImage->mType;
    imageFile >> ppmImage->nVertex;
    imageFile >> ppmImage->nFace;
    imageFile >> ppmImage->nEdge;

    auto i = 0;
    while (i < (ppmImage->nVertex )) {

        float x, y, z;

        imageFile >> x;
        imageFile >> y;
        imageFile >> z;
        Vertex temp(x, y, z);
        ppmImage->mVertex.push_back(temp);

        ++i;
    }

    auto j = 0;

    while (j < (ppmImage->nFace)) {
        unsigned int n, a, b, c;
        imageFile >> n;
        imageFile >> a;
        imageFile >> b;
        imageFile >> c;
        Indice temp(a, b, c);
        ppmImage->mIndice.push_back(temp);

        ++j;
    }

    imageFile.close();

    return ppmImage;
}

#endif