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

const int maxPixel=2000;
const int maxObjects=100;
int textureMode;
Color **textureBuffer;
int texHeight, texWidth;
double cameraAngle = 0.03;

// point pos(100,100,0), u(0,0,1), r(-.707,.707,0), l(-.707,-.707,0);

typedef std::pair<int,int> pii;

bool isIlluminated(Scene *scene, point p, point vec){
    double tempT[maxObjects];
    for(int i = 0;i<scene->numOfObjects;i++){
        tempT[i] = scene->objectArr[i]->getT(p, vec).first;
    }

    bool allAreMinus=true;
    for(int i=0;i<scene->numOfObjects;i++) if(tempT[i]!=-1) allAreMinus=false;

    return allAreMinus;
}

bool isIlluminatedSpotLight(Scene *scene, point p, point vec,SpotLight spl){
    if(isIlluminated(scene, p, vec)==false) return false;

    point srcToPoint=getUnitVec(spl.aPoint,p);
    point srcTolk=getUnitVec(spl.aPoint,spl.look);
    double angle=acos(getDotProd(srcToPoint,srcTolk));
    angle=angle*180.0/pi;
    if (angle>spl.cutoff) return false;
    return true;
}

void genPointBuffer(Scene *scene, Camera *camera, point** pointBuffer) {
    bool visited[maxPixel][maxPixel];

    double screenY = 2 * scene->nearPlane * tan(scene->fovy * pi / 360.0);
    double fovx = scene->fovy * scene->aspectRatio;
    double screenX = 2 * scene->nearPlane * tan(fovx*pi/360.0);

    point midPoint = camera->pos + camera->l * scene->nearPlane;

    double pixelWidY=screenY/scene->numOfPixels;
    double pixelWidX=screenX/scene->numOfPixels;

    int dirX[]={1,-1, 0, 0};
    int dirY[]={0, 0, 1,-1};

    memset(visited, false, sizeof(visited));
    std::queue<pii> q;

    pii start(scene->numOfPixels/2, scene->numOfPixels/2);
    q.push(start);
    visited[start.first][start.second]=true;
    pointBuffer[start.first][start.second]=midPoint;

    while(!q.empty()){
        pii v=q.front();
        q.pop();
        for(int i=0;i<4;i++){
            pii temp(v.first+dirY[i], v.second+dirX[i]);
            if(visited[temp.first][temp.second]==false && (temp.first>=0 && temp.first<scene->numOfPixels) &&
               (temp.second>=0 && temp.second<scene->numOfPixels)){

                visited[temp.first][temp.second]=true;
                if (i==0) pointBuffer[temp.first][temp.second]=pointBuffer[v.first][v.second]+camera->r*pixelWidX;
                else if (i==1) pointBuffer[temp.first][temp.second]=pointBuffer[v.first][v.second]+camera->r*(-pixelWidX);
                else if (i==2) pointBuffer[temp.first][temp.second]=pointBuffer[v.first][v.second]+camera->u*pixelWidY;
                else if (i==3) pointBuffer[temp.first][temp.second]=pointBuffer[v.first][v.second]+camera->u*(-pixelWidY);
                q.push(temp);
            }
        }
    }

}

Color getPixelColor(point p,point vec,int depth, Scene *scene) {  //return value 0 represents black and 1 represents white color

    Color black(0.0,0.0,0.0);
    Color white(1.0,1.0,1.0);

    //if(depth==0) return black;
    Color finalColor;
    int typeOfPixel;
    //typeOfPixel=-2 means blank -1 means checkerboard and another positive value means respective
    //index of objectArr

    double minT=100000,minIdx,tempT[maxObjects];
    point normalArr[maxObjects];
    point normalOnCheck(0,0,1);
    //stores the normal vector

    for(int i = 0; i < scene->numOfObjects; i++) {
        pdp tempPair = scene->objectArr[i]->getT(p,vec);
        tempT[i]=tempPair.first;
        normalArr[i]=tempPair.second;
    }

    bool allAreMinus=true;
    for(int i=0;i<scene->numOfObjects;i++) if(tempT[i]!=-1) allAreMinus=false;

    if (allAreMinus) minT=-1;
    else {
        for(int i=0;i<scene->numOfObjects;i++){
            if(tempT[i]!=-1 && tempT[i]<minT){
                minT=tempT[i];
                minIdx=i;
            }
        }
    }

    if(vec.z==0){
        if(minT==-1) {finalColor=black; typeOfPixel=-2;}
        else {finalColor=scene->objectArr[minIdx]->c; typeOfPixel=minIdx;}
    }
    else {
        double checkerT=-p.z/vec.z;
        point pointOnChecker=p+vec*checkerT;

        if(getDistOfPoints(p,pointOnChecker)>scene->farPlane || checkerT<0) checkerT=-1;

        if(checkerT==-1){
            if(minT==-1) {finalColor=black; typeOfPixel=-2;}
            else {finalColor=scene->objectArr[minIdx]->c; typeOfPixel=minIdx;}
        }
        else {
            double checkerWidth = scene->board.width;
            int y=int((pointOnChecker.y+inf*checkerWidth)/checkerWidth)-inf;
            int x=int((pointOnChecker.x+inf*checkerWidth)/checkerWidth)-inf;

            int colStart,colorType;
            Color texColor;
            if(textureMode==1){
                double texH=(pointOnChecker.y+inf*checkerWidth)-((y+inf)*checkerWidth);
                double texW=(pointOnChecker.x+inf*checkerWidth)-((x+inf)*checkerWidth);
                int hIdx=int(texH*texHeight/checkerWidth);
                int wIdx=int(texW*texWidth/checkerWidth);
                texColor=textureBuffer[wIdx][hIdx];
            }
            else{
                if(abs(x)%2==0) colStart=0;
                else colStart=1;

                if(abs(y)%2==0) colorType=colStart;
                else colorType=1-colStart;
            }

            if(minT==-1){
                minT=checkerT;
                if(textureMode==1) {finalColor=texColor; typeOfPixel=-1;}
                else if(colorType==0) {finalColor=black; typeOfPixel=-1;}
                else if(colorType==1) {finalColor=white; typeOfPixel=-1;}
            }
            else {
                if (checkerT<minT){
                    minT=checkerT;
                    if(textureMode==1) {finalColor=texColor; typeOfPixel=-1;}
                    else if(colorType==0) {finalColor=black; typeOfPixel=-1;}
                    else if(colorType==1) {finalColor=white; typeOfPixel=-1;}
                }
                else {finalColor=scene->objectArr[minIdx]->c; typeOfPixel=minIdx;}
            }
        }

    }

    if (typeOfPixel==-2) return black;

    Color lightedColor=finalColor;
    Color reflectedColor(0,0,0);
    point reflectionPoint,pixelPoint, lightPos;
    point intersectionPoint=p+vec*minT;
    point vecToSource;
    double fall;
    bool decision;

    int numOfLights = scene->lightArr.size();
    int numOfSpotLights = scene->spotLightArr.size();

    double lambert=0.0, phong=0.0;
    for(int i=0;i<numOfLights+numOfSpotLights;i++){
        pixelPoint=intersectionPoint-vec*0.01;

        if (i<numOfLights){
            Light light=scene->lightArr[i];
            lightPos=light.aPoint;
            fall=light.falloff;
            vecToSource=getUnitVec(pixelPoint,lightPos);
            decision=isIlluminated(scene, pixelPoint,vecToSource);
        }
        else{
            SpotLight spotLight=scene->spotLightArr[i-numOfLights];
            lightPos=spotLight.aPoint;
            fall=spotLight.falloff;
            vecToSource=getUnitVec(pixelPoint,lightPos);
            decision=isIlluminatedSpotLight(scene, pixelPoint,vecToSource,spotLight);
        }
        point reflected(0,0,0);


        if (decision){
            double d=getDistOfPoints(pixelPoint,lightPos);
            double scaleFact=exp(-d*d*fall);
            if(typeOfPixel>=0){
                point normal=normalArr[typeOfPixel];
                lambert+=std::max(getDotProd(vecToSource,normal)*scaleFact,0.0);

                double factor=2*getDotProd(vecToSource,normal);
                reflected=normal*factor+vecToSource*(-1);
                reflected.normalize();
                phong+=std::max(pow(getDotProd(reflected,vec*(-1)),scene->objectArr[typeOfPixel]->shininess)*scaleFact, 0.0);
            }
            else if(typeOfPixel==-1){
                lambert+=std::max(getDotProd(vecToSource,normalOnCheck)*scaleFact, 0.0);
            }
        }
    }
    point reflectionRay;
    if (depth < scene->levelOfRecursion) {
        if (typeOfPixel==-1)
            reflectionRay=vec+normalOnCheck*(-2*getDotProd(vec,normalOnCheck));
        else if(typeOfPixel>=0){
            point normal=normalArr[typeOfPixel];
            reflectionRay=vec+normal*(-2*getDotProd(vec,normal));
        }
        reflectionRay.normalize();
        reflectedColor=getPixelColor(intersectionPoint+reflectionRay*0.0001,reflectionRay,depth+1, scene); ;
    }

    if(typeOfPixel==-1) lightedColor=finalColor*scene->board.ambient
        +finalColor*(lambert*scene->board.diffuse)
        +reflectedColor*scene->board.reflection;


    else if (typeOfPixel>=0) lightedColor=
         finalColor*scene->objectArr[typeOfPixel]->ambient
        +finalColor*(lambert*scene->objectArr[typeOfPixel]->diffuse)
        +finalColor*(phong*scene->objectArr[typeOfPixel]->specular)
        +reflectedColor*scene->objectArr[typeOfPixel]->reflection;


    return lightedColor;
}



void traceTheRay(Scene *scene, Camera *camera){
    
    int numOfPixels = scene->numOfPixels;
    Color background(0.0,0.0,0.0);
    Color white(1.0,1.0,1.0);
    Color** pixels = new Color*[numOfPixels];
    for (int i=0;i<numOfPixels;i++) {
        pixels[i]=new Color[numOfPixels];
        for (int j=0;j<numOfPixels;j++)
            pixels[i][j]=background;
    }

    printf("Done 1\n");



    // texHeight = b_img.height();
    // texWidth = b_img.width();
    // textureBuffer = new Color* [texWidth];
    // for (int i = 0; i < texWidth; i++) {
    //     textureBuffer[i] = new Color [texHeight];
    //     for (int j = 0; j < texHeight; j++) {
    //         unsigned char r, g, b;
    //         b_img.get_pixel(i, j, r, g, b);
    //         Color c(r/255.0, g/255.0, b/255.0);
    //         textureBuffer[i][j] = c;
    //     }
    // }

    printf("Done 2\n");
    point** pointBuffer = new point*[maxPixel];
    for (int i = 0; i < maxPixel; ++i)
        pointBuffer[i] = new point[maxPixel];

    genPointBuffer(scene, camera, pointBuffer);

    printf("Done 3\n");

    for(int i=0;i<numOfPixels;i++){
        for(int j=0;j<numOfPixels;j++){
            point gridPoint=pointBuffer[i][j];
            point ray=getUnitVec(camera->pos,gridPoint);
            pixels[j][numOfPixels-i-1]=getPixelColor(gridPoint,ray,0, scene);
        }
    }

    printf("Done 4\n");

    bitmap_image image(numOfPixels,numOfPixels);
    for (int i=0;i<numOfPixels;i++) {
        for (int j=0;j<numOfPixels;j++) {
            image.set_pixel(i, j, std::min(pixels[i][j].r*255,255.0), std::min(pixels[i][j].g*255,255.0), std::min(pixels[i][j].b*255,255.0));
        }
    }
    image.save_image("out.bmp");

    for (int i = 0; i < maxPixel; ++i)
        delete[] pointBuffer[i];
    delete[] pointBuffer;

    // for (int i = 0; i < numOfPixels; i++) {
    //     delete[] pixels[i];
    // }
    // delete[] pixels;

    printf("Ray tracing completed\n");
}



#endif