#ifndef SCENE_H
#define SCENE_H

#include <vector>
#include <string>
#include "object.h"
#include "light.h"
#include <fstream>
#include "checker_board.h"


class Scene {
public:
    double nearPlane, farPlane, fovy, aspectRatio;
    int levelOfRecursion, numOfPixels;
    int numOfObjects;

    CheckerBoard board;
    std::vector<Object*> objectArr;
    std::vector<Light> lightArr;
    std::vector<SpotLight> spotLightArr;

    Scene() = default;

    void loadFromFile(const std::string& fileName);
};


void Scene::loadFromFile(const std::string& fileName) {
    std::ifstream inFile(fileName.c_str());

    double checkerWidth, checkerAmbient, checkerDiffuse, checkerReflection;

    inFile >> nearPlane >> farPlane >> fovy >> aspectRatio;
    inFile >> levelOfRecursion >> numOfPixels;
    inFile >> checkerWidth >> checkerAmbient >> checkerDiffuse >> checkerReflection;

    inFile >> numOfObjects;

    std::string objType;
    double xx, yy, zz, rr, gg, bb, a, d, s, r, rad, wid, hi, shini, falof, cutof, lx, ly, lz;

    board = CheckerBoard(checkerWidth, checkerAmbient, checkerDiffuse, checkerReflection);

    for (int i = 0; i < numOfObjects; i++) {
        inFile >> objType;
        if (objType == "sphere") {
            inFile >> xx >> yy >> zz;
            point tempPoint(xx, yy, zz);
            inFile >> rad;
            inFile >> rr >> gg >> bb;
            Color tempClr(rr, gg, bb);
            inFile >> a >> d >> s >> r;
            inFile >> shini;
            Sphere* sphere = new Sphere(tempPoint, tempClr, a, d, s, r, shini, rad);
            objectArr.push_back(sphere);
        } else if (objType == "pyramid") {
            inFile >> xx >> yy >> zz;
            point tempPoint(xx, yy, zz);
            inFile >> wid >> hi;
            inFile >> rr >> gg >> bb;
            Color tempClr(rr, gg, bb);
            inFile >> a >> d >> s >> r;
            inFile >> shini;
            Pyramid* pyramid = new Pyramid(tempPoint, tempClr, a, d, s, r, shini, wid, hi);
            objectArr.push_back(pyramid);
        }
    }

    int numOfLights;
    inFile >> numOfLights;
    for (int i = 0; i < numOfLights; i++) {
        inFile >> xx >> yy >> zz >> falof;
        point tempPoint(xx, yy, zz);
        Light tempLight(tempPoint, falof);
        lightArr.push_back(tempLight);
    }

    int numOfSpotLights;
    inFile >> numOfSpotLights;
    for (int i = 0; i < numOfSpotLights; i++) {
        inFile >> xx >> yy >> zz >> falof;
        inFile >> lx >> ly >> lz >> cutof;
        point tempPoint(xx, yy, zz);
        point tempLook(lx, ly, lz);
        SpotLight tempLight(tempPoint, falof, tempLook, cutof);
        spotLightArr.push_back(tempLight);
    }

    inFile.close();
}

#endif // SCENE_H
