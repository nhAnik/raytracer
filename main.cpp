#include<stdio.h>
#include<stdlib.h>
#include<math.h>
#include <GL/glut.h>
#include<vector>
#include<utility>
#include<fstream>
#include<string.h>
#include<queue>
#include <filesystem>

#include "point.h"
#include "checker_board.h"
#include "color.h"
#include "object.h"
#include "light.h"
#include "scene.h"
#include "ray_tracer.h"
#include "camera.h"
#include "bitmap_image.hpp"

Scene *scene;
Camera *camera;
std::string inputFile, outputFile, textureFile;

void keyboardListener(unsigned char key, int x,int y) {
	switch(key) {
        case '0':
            traceRays(scene, camera, outputFile, textureFile);
            break;

	    case '1':
			camera->l = rotateVec(camera->l, getCrossProd(camera->u, camera->l).get_norm());
			camera->r = rotateVec(camera->r, getCrossProd(camera->u, camera->r).get_norm());
			break;

		case '2':
            camera->l = rotateVec(camera->l, getCrossProd(camera->l, camera->u).get_norm());
			camera->r = rotateVec(camera->r, getCrossProd(camera->r, camera->u).get_norm());
			break;

        case '3':
            camera->l = rotateVec(camera->l, getCrossProd(camera->r, camera->l).get_norm());
			camera->u = rotateVec(camera->u, getCrossProd(camera->r, camera->u).get_norm());
            break;

        case '4':
            camera->l = rotateVec(camera->l, getCrossProd(camera->l, camera->r).get_norm());
			camera->u = rotateVec(camera->u, getCrossProd(camera->u, camera->r).get_norm());
            break;

        case '5': //counterclock
            camera->u = rotateVec(camera->u, getCrossProd(camera->l, camera->u).get_norm());
			camera->r = rotateVec(camera->r, getCrossProd(camera->l, camera->r).get_norm());
            break;

        case '6':
            camera->u = rotateVec(camera->u, getCrossProd(camera->u, camera->l).get_norm());
			camera->r = rotateVec(camera->r, getCrossProd(camera->r, camera->r).get_norm());
            break;

        case ' ':
            if (!textureFile.empty()) {
                textureMode=1-textureMode;
                if (textureMode==1) printf("Texture Mode Enabled\n");
                else if (textureMode==0) printf("Texture Mode Disabled\n");
            }

		default:
			break;
	}
}

void specialKeyListener(int key, int x,int y) {
	switch(key) {
	    case GLUT_KEY_UP: 
            // up arrow
            camera->pos = camera->pos + camera->l;
			break;

		case GLUT_KEY_DOWN:
            // down arrow
            camera->pos = camera->pos - camera->l;
			break;

		case GLUT_KEY_RIGHT:
            // right arrow
            camera->pos = camera->pos + camera->r;
			break;

		case GLUT_KEY_LEFT:
            // left arrow
            camera->pos = camera->pos - camera->r;
			break;

		case GLUT_KEY_PAGE_UP:
            // page up
            camera->pos = camera->pos + camera->u;
			break;

		case GLUT_KEY_PAGE_DOWN:
            // page down
            camera->pos = camera->pos - camera->u;
			break;

		default:
			break;
	}
}

void display() {
	// clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0, 0, 0, 0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	// load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	// initialize the matrix
	glLoadIdentity();

	// three info
	// 1. where is the camera (viewer)?
	// 2. where is the camera looking?
	// 3. Which direction is the camera's UP direction?
	gluLookAt(
        camera->pos.x, camera->pos.y, camera->pos.z,
        camera->pos.x + camera->l.x, camera->pos.y + camera->l.y, camera->pos.z + camera->l.z,
        camera->u.x, camera->u.y, camera->u.z
    );

	// again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);

	// draw the scene
    scene->draw();

	// ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}

void animate() {
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(const std::string& input) {
	// clear the screen
	glClearColor(0,0,0,0);

    scene = new Scene();
    scene->loadFromFile(input);

    camera = new Camera(
        point(100,100,0),
        point(0,0,1),
        point(-.707,.707,0),
        point(-.707,-.707,0)
    );

	/************************
	/ set-up projection here
	************************/
	// load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	// initialize the matrix
	glLoadIdentity();

	// give PERSPECTIVE parameters
    // field of view in the Y (vertically)
	// aspect ratio that determines the field of view in the X direction (horizontally)
	// near distance
	// far distance
    gluPerspective(scene->fovy, scene->aspectRatio, scene->nearPlane, scene->farPlane);
	
}

int main(int argc, char **argv) {
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];

        if (arg == "--input" && i + 1 < argc) {
            inputFile = argv[++i];
        } else if (arg == "--output" && i + 1 < argc) {
            outputFile = argv[++i];
        } else if (arg == "--texture" && i + 1 < argc) {
            textureFile = argv[++i];
        } else {
            std::cerr << "Unknown or malformed argument: " << arg << "\n";
            return 1;
        }
    }

    if (inputFile.empty() || outputFile.empty()) {
        std::cerr << "Error: --input and --output are required.\n";
        return 1;
    }

    if (!std::filesystem::exists(inputFile)) {
        std::cerr << "Error: input file '" << inputFile << "' does not exist.\n";
        return 1;
    }

    if (!textureFile.empty() && !std::filesystem::exists(textureFile)) {
        std::cerr << "Error: texture file '" << textureFile << "' does not exist.\n";
        return 1;
    }

	glutInit(&argc,argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	// Depth, Double buffer, RGB color

	glutCreateWindow("Ray Tracer");

	init(inputFile);

	glEnable(GL_DEPTH_TEST);	// enable Depth Testing

	glutDisplayFunc(display);	// display callback function
	glutIdleFunc(animate);		// what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);

	glutMainLoop();		// The main loop of OpenGL

	return 0;
}
