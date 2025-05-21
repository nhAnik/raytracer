#ifndef RAYTRACERH
#define RAYTRACERH

#include <math.h>
#include <queue>
#include <stdlib.h>
#include "point.h"
#include "color.h"
#include "scene.h"
#include "bitmap_image.hpp"
#include "camera.h"
#include "checker_board.h"

#define pi (2*acos(0.0))


const int maxPixel=2000;
const int maxObjects=100;
int textureMode = 0;

double cameraAngle = 0.03;

typedef std::pair<int,int> pii;

bool isIlluminated(Scene *scene, point p, point vec) {
    for (auto& object : scene->objectArr) {
        if (object->getIntersection(p, vec).first != -1) {
            return false;
        }
    }

    return true;
}

bool isIlluminatedBySpotLight(Scene *scene, point p, point vec,SpotLight spl) {
    if (!isIlluminated(scene, p, vec)) {
        return false;
    }

    point srcToPoint = getUnitVec(spl.aPoint,p);
    point srcTolk = getUnitVec(spl.aPoint,spl.look);
    double angle = acos(getDotProd(srcToPoint,srcTolk));
    angle = angle*180.0/pi;
    if (angle > spl.cutoff) {
        return false;
    }
    return true;
}

void generatePointBuffer(Scene *scene, Camera *camera, point** pointBuffer) {
    double screenY = 2 * scene->nearPlane * tan(scene->fovy * pi / 360.0);
    double fovx = scene->fovy * scene->aspectRatio;
    double screenX = 2 * scene->nearPlane * tan(fovx*pi/360.0);

    point midPoint = camera->pos + camera->l * scene->nearPlane;

    double pixelWidY=screenY/scene->numOfPixels;
    double pixelWidX=screenX/scene->numOfPixels;

    int dirX[]={1,-1, 0, 0};
    int dirY[]={0, 0, 1,-1};

    bool visited[maxPixel][maxPixel];
    memset(visited, false, sizeof(visited));

    std::queue<pii> q;
    pii start(scene->numOfPixels/2, scene->numOfPixels/2);
    q.push(start);
    visited[start.first][start.second] = true;
    pointBuffer[start.first][start.second] = midPoint;

    while (!q.empty()) {
        pii v = q.front();
        q.pop();

        for (int i = 0; i < 4; i++) {
            pii next(v.first + dirY[i], v.second + dirX[i]);
            if (!visited[next.first][next.second] &&
                (next.first >= 0 && next.first < scene->numOfPixels) &&
               (next.second >= 0 && next.second < scene->numOfPixels)
            ) {
                visited[next.first][next.second]=true;

                point toAdd = point(0, 0, 0);
                if (i == 0) toAdd = camera->r * pixelWidX;
                else if (i== 1) toAdd = camera->r * (-pixelWidX);
                else if (i == 2) toAdd = camera->u * pixelWidY;
                else if (i == 3) toAdd = camera->u * (-pixelWidY);

                pointBuffer[next.first][next.second] = pointBuffer[v.first][v.second] + toAdd;
                q.push(next);
            }
        }
    }

}

//return value 0 represents black and 1 represents white color
Color getPixelColor(point p, point vec, int depth, Scene *scene, Color **textureBuffer, int texHeight, int texWidth) {

    Color black(0.0,0.0,0.0);
    Color white(1.0,1.0,1.0);
    Color finalColor;

    // If the value of typeOfPixel is -2, it means blank. On the other hand, -1 means checkerboard.
    // If the value is non-negative, it means respective index of objectArr.
    int typeOfPixel;

    double minT = 100000, minIdx, tempT[maxObjects];

    //stores the normal vector
    point normalArr[maxObjects];
    point normalOnCheck(0,0,1);

    for(int i = 0; i < scene->numOfObjects; i++) {
        pdp tempPair = scene->objectArr[i]->getIntersection(p,vec);
        tempT[i] = tempPair.first;
        normalArr[i] = tempPair.second;
    }

    bool allAreMinus=true;
    for (int i = 0; i < scene->numOfObjects; i++) {
        if (tempT[i] != -1) {
            allAreMinus = false;
        }
    }

    if (allAreMinus) {
        minT=-1;
    } else {
        for (int i = 0; i < scene->numOfObjects; i++) {
            if (tempT[i] != -1 && tempT[i] < minT) {
                minT=tempT[i];
                minIdx=i;
            }
        }
    }

    if (vec.z == 0) {
        if (minT == -1) {
            finalColor=black;
            typeOfPixel=-2;
        } else {
            finalColor=scene->objectArr[minIdx]->c;
            typeOfPixel=minIdx;
        }
    } else {
        double checkerT=-p.z/vec.z;
        point pointOnChecker=p+vec*checkerT;

        if (getDistOfPoints(p,pointOnChecker)>scene->farPlane || checkerT<0) {
            checkerT=-1;
        }

        if (checkerT==-1) {
            if (minT == -1) {
                finalColor=black;
                typeOfPixel=-2;
            } else {
                finalColor=scene->objectArr[minIdx]->c;
                typeOfPixel=minIdx;
            }
        } else {
            double checkerWidth = scene->board.width;
            int y=int((pointOnChecker.y+inf*checkerWidth)/checkerWidth)-inf;
            int x=int((pointOnChecker.x+inf*checkerWidth)/checkerWidth)-inf;

            int colStart,colorType;
            Color texColor;
            if (textureMode == 1) {
                double texH = (pointOnChecker.y + inf*checkerWidth) - ((y + inf) * checkerWidth);
                double texW = (pointOnChecker.x + inf*checkerWidth) - ((x + inf) * checkerWidth);
                int hIdx = int(texH * texHeight / checkerWidth);
                int wIdx = int(texW * texWidth / checkerWidth);
                texColor=textureBuffer[wIdx][hIdx];
            } else {
                if (abs(x)%2 == 0) colStart=0;
                else colStart=1;

                if (abs(y)%2 == 0) colorType=colStart;
                else colorType=1-colStart;
            }

            if (minT == -1) {
                minT = checkerT;
                if (textureMode == 1) {
                    finalColor = texColor;
                    typeOfPixel = -1;
                } else if (colorType == 0) {
                    finalColor = black; 
                    typeOfPixel = -1;
                } else if (colorType == 1) {
                    finalColor = white;
                    typeOfPixel = -1;
                }
            } else {
                if (checkerT < minT) {
                    minT=checkerT;
                    if (textureMode == 1) {
                        finalColor = texColor;
                        typeOfPixel = -1;
                    } else if (colorType == 0) {
                        finalColor = black;
                        typeOfPixel = -1;
                    } else if (colorType == 1) {
                        finalColor = white;
                        typeOfPixel = -1;
                    }
                } else {
                    finalColor = scene->objectArr[minIdx]->c;
                    typeOfPixel = minIdx;
                }
            }
        }

    }

    if (typeOfPixel == -2) {
        return black;
    }

    Color lightedColor = finalColor;
    Color reflectedColor(0,0,0);
    point reflectionPoint,pixelPoint, lightPos;
    point intersectionPoint = p + vec*minT;
    point vecToSource;
    double fall;
    bool decision;

    int numOfLights = scene->lightArr.size();
    int numOfSpotLights = scene->spotLightArr.size();

    double lambert=0.0, phong=0.0;
    for(int i = 0; i < numOfLights+numOfSpotLights; i++) {
        pixelPoint = intersectionPoint - vec*0.01;

        if (i < numOfLights) {
            Light light = scene->lightArr[i];
            lightPos = light.aPoint;
            fall = light.falloff;
            vecToSource = getUnitVec(pixelPoint,lightPos);
            decision = isIlluminated(scene, pixelPoint, vecToSource);
        } else{
            SpotLight spotLight = scene->spotLightArr[i-numOfLights];
            lightPos = spotLight.aPoint;
            fall = spotLight.falloff;
            vecToSource = getUnitVec(pixelPoint,lightPos);
            decision = isIlluminatedBySpotLight(scene, pixelPoint, vecToSource, spotLight);
        }

        point reflected(0,0,0);

        if (decision) {
            double d = getDistOfPoints(pixelPoint,lightPos);
            double scaleFact = exp(-d*d*fall);
            if (typeOfPixel >= 0) {
                point normal = normalArr[typeOfPixel];
                lambert += std::max(getDotProd(vecToSource,normal)*scaleFact,0.0);

                double factor = 2*getDotProd(vecToSource,normal);
                reflected = normal*factor+vecToSource*(-1);
                reflected.normalize();
                phong += std::max(pow(getDotProd(reflected,vec*(-1)), scene->objectArr[typeOfPixel]->shininess)*scaleFact, 0.0);
            }
            else if (typeOfPixel==-1) {
                lambert += std::max(getDotProd(vecToSource,normalOnCheck)*scaleFact, 0.0);
            }
        }
    }

    point reflectionRay;
    if (depth < scene->levelOfRecursion) {
        if (typeOfPixel == -1)
            reflectionRay = vec + normalOnCheck * (-2*getDotProd(vec,normalOnCheck));
        else if (typeOfPixel >= 0) {
            point normal = normalArr[typeOfPixel];
            reflectionRay = vec + normal * (-2*getDotProd(vec,normal));
        }
        reflectionRay.normalize();
        reflectedColor = getPixelColor(
            intersectionPoint + reflectionRay*0.0001,
            reflectionRay,
            depth+1,
            scene,
            textureBuffer,
            texHeight,
            texWidth
        );
    }

    if (typeOfPixel == -1) {
        lightedColor = finalColor*scene->board.ambient
            + finalColor*(lambert*scene->board.diffuse)
            + reflectedColor*scene->board.reflection;
    } else if (typeOfPixel >= 0) {
        lightedColor = finalColor*scene->objectArr[typeOfPixel]->ambient
            + finalColor*(lambert*scene->objectArr[typeOfPixel]->diffuse)
            + finalColor*(phong*scene->objectArr[typeOfPixel]->specular)
            + reflectedColor*scene->objectArr[typeOfPixel]->reflection;
    }

    return lightedColor;
}


void traceRays(Scene *scene, Camera *camera, const std::string& output, const std::string& texture) {
    int numOfPixels = scene->numOfPixels;
    Color background(0.0,0.0,0.0);
    Color white(1.0,1.0,1.0);
    Color** pixels = new Color*[numOfPixels];
    for (int i = 0; i < numOfPixels; i++) {
        pixels[i] = new Color[numOfPixels];
        for (int j = 0; j < numOfPixels; j++) {
            pixels[i][j] = background;
        }
    }

    Color **textureBuffer;
    int texHeight, texWidth;

    if (textureMode == 1) {
        bitmap_image textureImage(texture);
        texHeight = textureImage.height();
        texWidth = textureImage.width();
        textureBuffer = new Color*[texWidth];
        for (int i = 0; i < texWidth; i++) {
            textureBuffer[i] = new Color [texHeight];
            for (int j = 0; j < texHeight; j++) {
                unsigned char r, g, b;
                textureImage.get_pixel(i, j, r, g, b);
                textureBuffer[i][j] = Color(r/255.0, g/255.0, b/255.0);
            }
        }
    }

    point** pointBuffer = new point*[maxPixel];
    for (int i = 0; i < maxPixel; ++i)
        pointBuffer[i] = new point[maxPixel];

    generatePointBuffer(scene, camera, pointBuffer);

    for(int i = 0; i < numOfPixels; i++) {
        for(int j = 0; j < numOfPixels; j++) {
            point gridPoint = pointBuffer[i][j];
            point ray = getUnitVec(camera->pos,gridPoint);
            pixels[j][numOfPixels-i-1] = getPixelColor(gridPoint,ray,0, scene, textureBuffer, texHeight, texWidth);
        }
    }

    bitmap_image image(numOfPixels,numOfPixels);
    for (int i=0;i<numOfPixels;i++) {
        for (int j=0;j<numOfPixels;j++) {
            image.set_pixel(i, j,
                std::min(pixels[i][j].r*255 , 255.0),
                std::min(pixels[i][j].g*255 , 255.0),
                std::min(pixels[i][j].b*255 , 255.0)
            );
        }
    }
    image.save_image(output);

    for (int i = 0; i < maxPixel; ++i)
        delete[] pointBuffer[i];
    delete[] pointBuffer;

    for (int i = 0; i < numOfPixels; i++) {
        delete[] pixels[i];
    }
    delete[] pixels;

    printf("Output generated!\n");
}

#endif
