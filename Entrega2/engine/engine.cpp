#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <stdlib.h>
#include <GL/glut.h>
#endif

#include "tinyxml/tinyxml2.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>

using namespace tinyxml2;
using namespace std;

int window;

// Global Variables
// Window Size
int wHeight = 800;
int wWidth = 800;

// Câmera
// Position
float px = 5.0f;
float py = 5.0f;
float pz = 5.0f;

// LookAt
float lx = 0.0f;
float ly = 0.0f;
float lz = 0.0f;

// Up vector
float ux = 0.0f;
float uy = 1.0f;
float uz = 0.0f;

// Projection
float fov = 45.0f;
float near = 1.0f;
float far = 1000.0f;

// Velocidade da câmera
float speed = 0.1;
float rotSpeed = 0.0005;

float a = 0.0f, b = 0.0f, r = 0.0f;

// Ficheiros e vértices
std::vector<float> vertex;
std::vector<string> files;

void changeSize(int w, int h)
{

	// Prevent a divide by zero, when window is too short
	// (you cant make a window with zero width).
	if (h == 0)
		h = 1;

	// compute window's aspect ratio
	float ratio = w * 1.0 / h;

	// Set the projection matrix as current
	glMatrixMode(GL_PROJECTION);
	// Load Identity Matrix
	glLoadIdentity();

	// Set the viewport to be the entire window
	glViewport(0, 0, w, h);

	// Set perspective
	gluPerspective(fov, ratio, near, far);

	// return to the model view matrix mode
	glMatrixMode(GL_MODELVIEW);
}

void renderScene(void)
{

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(px, py, pz,
			  lx, ly, lz,
			  ux, uy, uz);

	// put axis drawing in here
	glBegin(GL_LINES);
	// X axis in red
	glColor3f(1.0f, 0.0f, 0.0f);
	glVertex3f(-100.0f, 0.0f, 0.0f);
	glVertex3f(100.0f, 0.0f, 0.0f);
	// Y Axis in Green
	glColor3f(0.0f, 1.0f, 0.0f);
	glVertex3f(0.0f, -100.0f, 0.0f);
	glVertex3f(0.0f, 100.0f, 0.0f);
	// Z Axis in Blue
	glColor3f(0.0f, 0.0f, 1.0f);
	glVertex3f(0.0f, 0.0f, -100.0f);
	glVertex3f(0.0f, 0.0f, 100.0f);
	glEnd();

	// put the geometric transformations here
	glBegin(GL_TRIANGLES);
	int size = vertex.size();
	for (int i = 0; i < size; i += 3)
	{
		glColor3f(1, 1, 1);
		glVertex3f(vertex[i], vertex[i + 1], vertex[i + 2]);
	}
	glEnd();

	// End of frame
	glutSwapBuffers();
}

void readFile()
{
	// Variáveis
	float x, y, z;
	string linha;
	for (string i : files)
	{
		// Abre o ficheiro a ser lido
		ifstream file("../../3d/" + i);
		while (std::getline(file, linha))
		{
			// Lê os pontos linha a linha e adiciona os vértices
			istringstream in(linha);
			in >> x;
			in >> y;
			in >> z;
			vertex.push_back(x);
			vertex.push_back(y);
			vertex.push_back(z);
		}
		file.close();
	}
}

int readXML()
{
	// Abre File
	XMLDocument config;
	XMLError eResult = config.LoadFile("../config.xml");
	// Procura a root do XML
	XMLElement *pRootElement = config.RootElement();
	// Termina se não existir a root
	if (pRootElement == NULL)
		return -1;
	// Procura o elemento window
	XMLElement *pWindow = pRootElement->FirstChildElement("window");
	// Guarda os valores da janela
	if (pWindow != NULL)
	{
		wWidth = int(pWindow->FindAttribute("width")->IntValue());
		wHeight = int(pWindow->FindAttribute("height")->IntValue());
	}
	// Procura o elemento camera
	XMLElement *pCamera = pRootElement->FirstChildElement("camera");
	// Guarda os valores referentes à câmera
	if (pCamera != NULL)
	{
		// Procura o elemento position
		XMLElement *pPosition = pCamera->FirstChildElement("position");
		// Guarda os valores referentes à posição da câmera
		if (pPosition != NULL)
		{
			px = float(pPosition->FindAttribute("x")->FloatValue());
			py = float(pPosition->FindAttribute("y")->FloatValue());
			pz = float(pPosition->FindAttribute("z")->FloatValue());
		}
		// Procura o elemento lookAt
		XMLElement *pLookAt = pCamera->FirstChildElement("lookAt");
		// Guarda os valores referentes ao lookAt da câmera
		if (pLookAt != NULL)
		{
			lx = float(pLookAt->FindAttribute("x")->FloatValue());
			ly = float(pLookAt->FindAttribute("y")->FloatValue());
			lz = float(pLookAt->FindAttribute("z")->FloatValue());
		}
		// Procura o elemento up
		XMLElement *pUp = pCamera->FirstChildElement("up");
		// Guarda os valores referentes ao vetor up da câmera
		if (pUp != NULL)
		{
			ux = float(pUp->FindAttribute("x")->FloatValue());
			uy = float(pUp->FindAttribute("y")->FloatValue());
			uz = float(pUp->FindAttribute("z")->FloatValue());
		}
		// Procura o elemento projection
		XMLElement *pProjection = pCamera->FirstChildElement("projection");
		// Guarda os valores referentes à projeção da câmera
		if (pProjection != NULL)
		{
			fov = float(pProjection->FindAttribute("fov")->FloatValue());
			near = float(pProjection->FindAttribute("near")->FloatValue());
			far = float(pProjection->FindAttribute("far")->FloatValue());
		}
	}
	// procura o elemento group
	XMLElement *pGroup = pRootElement->FirstChildElement("group");
	if (pGroup == NULL)
		return -1;
	// Procura o elemento models
	XMLElement *pModels = pGroup->FirstChildElement("models");
	if (pModels == NULL)
		return -1;
	// Procura o elemento models
	XMLElement *pModel = pModels->FirstChildElement("model");
	if (pModel == NULL)
		return -1;
	// Guarda os ficheiros que serão lidos
	while (pModel != NULL)
	{
		std::string model = pModel->FindAttribute("file")->Value();
		files.push_back(model);
		pModel = pModel->NextSiblingElement("model");
	}


	r = sqrt(pow(px, 2) + pow(py, 2) + pow(pz, 2));
	a = acos((px * px) / ((sqrt(px * px)) * sqrt(px * px + py * py)));
	b = acos((px * px) / ((sqrt(px * px)) * sqrt(px * px + pz * pz)));

	return 1;
}

void processSpecialKeys(int key, int x, int y)
{
	switch (key) {
        case GLUT_KEY_RIGHT:
            a += 0.1; break;
        case GLUT_KEY_LEFT:
            a -= 0.1; break;
        case GLUT_KEY_UP:
            b += 0.1f;
            if (b > 1.5f)
                b = 1.5f;
            break;
        case GLUT_KEY_DOWN:
            b -= 0.1f;
            if (b < -1.5f)
                b = -1.5f;
            break;
    }

	px = r * cos(b) * sin(a);
    py = r * sin(b);
    pz = r * cos(b) * cos(a);

}

void processNormalKeys(unsigned char key, int x, int y)
{
    if (key == 27)
    {
        glutDestroyWindow(window);
        exit(0);
    }
    switch (key) {
    case 'w':
        r -= 0.1f;
        if (r < 0.1f)
            r = 0.1f;
        break;
    case 's':
        r += 0.1f;
        break;
    }

    px = r * cos(b) * sin(a);
    py = r * sin(b);
    pz = r * cos(b) * cos(a);
}



int main(int argc, char **argv)
{
	// Ler o ficheiro com a configuração
	if (!readXML())
		return -1;
	readFile();
	// Init GLUT and the window
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(wWidth, wHeight);
	window = glutCreateWindow("CG@DI-UM");

	// Required callback registry
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	// put here the registration of the keyboard callbacks
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processNormalKeys);

	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	// enter GLUT's main cycle
	glutMainLoop();
}
