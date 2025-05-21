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

    void draw();
};

#endif // SCENE_H
