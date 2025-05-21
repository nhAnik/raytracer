#include "scene.h"

void Scene::draw() {
    // Draw checker board
    this->board.draw();

    // Draw all the objects
    for (auto& object : this->objectArr) {
        object->draw();
    }

    // Draw the lights
    Color lightColor(0.9,0.9,0.9);
    Color spotLightColor(0.2,0.9,0.9);
    // for (int i=0;i<numOfLights;i++) {
    //     Light lit=lightArr[i];
    //     drawSphere(lit.aPoint,5,lightColor);
    // }
    // for (int i=0;i<numOfSpotLights;i++) {
    //     SpotLight sptlit=spotLightArr[i];
    //     drawSphere(sptlit.aPoint,10,spotLightColor);
    // }
}

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
