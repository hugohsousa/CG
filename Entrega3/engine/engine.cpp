#include <cstdio>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glew.h>
#include <stdlib.h>
#include <GL/glut.h>
#endif

#include "tinyxml/tinyxml2.h"
#include <cstring>
#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cmath>
#include "classes.h"
#include <tuple>

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
float speed = 0.8;
float rotSpeed = 0.0005;

// Angulo para movimentar a camera
bool warp = false;
float ang = 0;
float ang1 = -5000;
//Verificar se o rato está na janela
bool mouseCaptured = true; 

// Estrutura para guardar os grupos
typedef struct node{
    Group* g;
    string label;
    vector<struct node*> next;
} *Tree;

//File struct
typedef struct ficheiro{
    string name;
    vector<float> vertex;
    GLuint index,size;
} *File;
//Files
vector<File> Vbos;
Tree groupTree;
int n_group = 0;

//Função que lê os ficheiros e desenha os vértices
File readFile(string file)
{
	// Variáveis
	float x, y, z;
	string linha;
	ifstream f("../3d/" + file);
    File vbo = new struct ficheiro;
    vbo->name = file;
    vbo->vertex.clear();

    // Abre o ficheiro a ser lido
    while (std::getline(f, linha))
    {
        // Lê os pontos linha a linha e adiciona os vértices
        istringstream in(linha);
        in >> x;
        in >> y;
        in >> z;
        vbo->vertex.push_back(x);
        vbo->vertex.push_back(y);
        vbo->vertex.push_back(z);
	}

    vbo->size = vbo->vertex.size()/3;
    Vbos.push_back(vbo);

    //Criar vbo
    glGenBuffers(1,&(vbo->index));
    glBindBuffer(GL_ARRAY_BUFFER,vbo->index);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * (vbo->vertex).size(), vbo->vertex.data(),GL_STATIC_DRAW);

    f.close();

    return vbo;
}

//Função para parse dos grupos
void groupParser(XMLElement *pGroup, Tree group) {
    
    //Variáveis
    float x,y,z,angle,time = 0.0f;
    bool align;
    string file = "";
    
    XMLElement *pSubGroup = pGroup->FirstChildElement();
    
    while(pSubGroup != NULL) {
        if(strcmp(pSubGroup->Value(),"transform") == 0) {
            XMLElement *pTransform = pSubGroup->FirstChildElement();
            while(pTransform != NULL) {
                //Procurar translação
                if(strcmp(pTransform->Value(),"translate") == 0) {
                    x = float(pTransform->FindAttribute("x")->FloatValue());
                    y = float(pTransform->FindAttribute("y")->FloatValue());
                    z = float(pTransform->FindAttribute("z")->FloatValue());
                    //Criar árvore auxiliar e guardar translação 
                    Tree aux = new struct node;
                    aux->g = new Translate(x,y,z);
                    aux->label = "translate";
                    aux->next.clear();
                    group->next.push_back(aux);
                }
                //Procurar translação
                if(strcmp(pTransform->Value(),"translate") == 0) {
                    //Contar o número de pontos >= 4
                    int number = 0;
                    //Parse nos pontos da curva 
                    XMLElement *pPoint = pTransform->FirstChildElement();
                    if(pPoint != NULL) {
                        while(pPoint != NULL) {
                            //Guardar os pontos
                            x = float(pPoint->FindAttribute("x")->FloatValue());
                            y = float(pPoint->FindAttribute("y")->FloatValue());
                            z = float(pPoint->FindAttribute("z")->FloatValue());
                            pPoint = pPoint->NextSiblingElement(); 
                        }
                        time = float(pTransform->FindAttribute("time")->FloatValue());
                        align = float(pTransform->FindAttribute("align")->BoolValue());
                        //Criar árvore auxiliar e guardar translação 
                        Tree aux = new struct node;
                        aux->g = new Translate(x,y,z);
                        aux->label = "translate";
                        aux->next.clear();
                        group->next.push_back(aux);
                    } else {
                        x = float(pTransform->FindAttribute("x")->FloatValue());
                        y = float(pTransform->FindAttribute("y")->FloatValue());
                        z = float(pTransform->FindAttribute("z")->FloatValue());
                        //Criar árvore auxiliar e guardar translação 
                        Tree aux = new struct node;
                        aux->g = new Translate(x,y,z);
                        aux->label = "translate";
                        aux->next.clear();
                        group->next.push_back(aux);
                    }
                }
                 //Procurar rotação
                if(strcmp(pTransform->Value(),"rotate") == 0) {
                    x = float(pTransform->FindAttribute("x")->FloatValue());
                    y = float(pTransform->FindAttribute("y")->FloatValue());
                    z = float(pTransform->FindAttribute("z")->FloatValue());
                    angle = float(pTransform->FindAttribute("angle")->FloatValue());
                    time = float(pTransform->FindAttribute("time")->FloatValue());
                    //Criar árvore auxiliar e guardar a rotação
                    Tree aux = new struct node;
                    aux->g = new Rotate(x,y,z,angle,time);
                    aux->label = "rotate";
                    aux->next.clear();
                    group->next.push_back(aux);
                }
                //Procurar escala
                if(strcmp(pTransform->Value(),"scale") == 0) {
                    x = float(pTransform->FindAttribute("x")->FloatValue());
                    y = float(pTransform->FindAttribute("y")->FloatValue());
                    z = float(pTransform->FindAttribute("z")->FloatValue());
                    //Criar árvore auxiliar e guardar a rotação
                    Tree aux = new struct node;
                    aux->g = new Scale(x,y,z);
                    aux->label = "scale";
                    aux->next.clear();
                    group->next.push_back(aux);
                }
                pTransform = pTransform->NextSiblingElement();
            }
        }
        //Procurar os ficheiros (modelos)
        if(strcmp(pSubGroup->Value(),"models") == 0){
            XMLElement* pModel = pSubGroup->FirstChildElement("model");
            while(pModel != NULL){
                const char * model = pModel->FindAttribute("file")->Value();
                //Criar árvore auxiliar e guardar o ficheiro (modelo)
                Tree aux = new struct node;
                aux->g = new Model(string(model));
                aux->label = "model";
                aux->next.clear();
                group->next.push_back(aux);
                pModel = pModel->NextSiblingElement("model");
            }
        }
        //Tratar dos subgroupos
        if(strcmp(pSubGroup->Value(),"group") == 0){
            //inicializar a subarvore do grupo para ser adicionada na arvore de classes
            Tree subGroup = new struct node;
            subGroup->g = new Group(n_group++);
            subGroup->label = "group";
            subGroup->next.clear();
            groupParser(pSubGroup, subGroup);
            group->next.push_back(subGroup);
        }
        pSubGroup = pSubGroup->NextSiblingElement();
    }
}

//Função que lê o ficheiro com a config
int readXML()
{
	// Abre File
	XMLDocument config;
	XMLError eResult = config.LoadFile("solar.xml");

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

    //Inicializar a árvore para guardar os grupos principais
    groupTree = new struct node;
    groupTree->g = new Group(n_group++);
    groupTree->label = "root";
    groupTree->next.clear();

    // procura o elemento group
    XMLElement *pGroup = pRootElement->FirstChildElement("group");

    //Percorre os grupos
    while (pGroup != NULL) {
        Tree group = new struct node;
        group->g = new Group(n_group++); 
        group->label = "group";
        groupParser(pGroup, group);
        groupTree->next.push_back(group);
        pGroup = pGroup->NextSiblingElement();
    }
	return 1;
}

//Função que lê a estrutura de dados e desenha a cena
int readTree(Tree groups) {
    if(groups == NULL) return -1;

    for(node *n : groups->next) {
        //Lê os subgroups de forma recursiva
        if(strcmp(n->label.c_str(),"group") == 0){
            glPushMatrix();
            readTree(n);
            glPopMatrix();
        }
        //Aplica a translação
        if(strcmp(n->label.c_str(),"translate") == 0){
            (n->g)->apply();
        }
        //Aplica a rotação
        if(strcmp(n->label.c_str(),"rotate") == 0){
            (n->g)->apply();
        }
        //Aplica a escala
        if(strcmp(n->label.c_str(),"scale") == 0){
            (n->g)->apply();
        }
        //Lê o model e guarda os vértices serão desenhados
        if(strcmp(n->label.c_str(),"model") == 0){
            int flag = 0;
            File aux;
            for(File vbo : Vbos){
                if(strcmp((dynamic_cast<Model*>(n->g))->getFile().c_str(),vbo->name.c_str()) == 0) {
                    aux = vbo;
                    flag = 1;
                    break;
                }
            }

            if (flag == 0){
                aux = readFile((dynamic_cast<Model*>(n->g))->getFile().c_str());
                Vbos.push_back(aux);
            }

            glBindBuffer(GL_ARRAY_BUFFER,aux->index);
            glVertexPointer(3,GL_FLOAT,0,0);
            glDrawArrays(GL_TRIANGLES,0,aux->size);

        }
    }
    return 1;
}

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

void processSpecialKeys(int key, int x, int y)
{
	switch (key) {
        case GLUT_KEY_UP:
            px += lx * speed;
            py += ly * speed;
            pz += lz * speed;
            break;
        case GLUT_KEY_DOWN:
            px -= lx * speed;
            py -= ly * speed;
            pz -= lz * speed;
            break;
        case GLUT_KEY_LEFT:
            px += lz * speed;
            pz -= lx * speed;
            break;
        case GLUT_KEY_RIGHT:
            px -= lz * speed;
            pz += lx * speed;
            break;
        }
}

void processNormalKeys(unsigned char key, int x, int y)
{
    //ESC Destroy window
    if (key == 27)
    {
        glutDestroyWindow(window);
        exit(0);
    }
}

void processMouse(int x, int y) {
    if (warp)
    {
        warp = false;
        return;
    }
    int dx = x - 100; 
    int dy = y - 100;
    ang = ang + dx * rotSpeed;
    ang1 = ang1 + dy * rotSpeed;
    lx = sin(ang1)*sin(ang);
    ly = -cos(ang1);
    lz = -sin(ang1)*cos(ang);
    if (mouseCaptured)
    {
        warp = true;
        glutWarpPointer(100, 100); 
    }	
}


void renderScene(void)
{

	// clear buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// set the camera
	glLoadIdentity();
	gluLookAt(px, py, pz,
			  px + lx, py + ly, pz + lz,
			  ux, uy, uz);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
    readTree(groupTree);
	// End of frame
	glutSwapBuffers();
}

int main(int argc, char **argv)
{
	// Init GLUT and the window 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(wWidth, wHeight);
	window = glutCreateWindow("CG@DI-UM");

    glewInit();
    readXML();

	// Required callback registry
	glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

	// put here the registration of the keyboard callbacks
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processNormalKeys);
    glutPassiveMotionFunc(processMouse);
    
	//  OpenGL settings
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);	
    glEnableClientState(GL_VERTEX_ARRAY);

	// enter GLUT's main cycle
	glutMainLoop();
}
