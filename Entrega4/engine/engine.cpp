#include <IL/il.h>
#include <set>
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
#include "headers/classes.h"
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

// Estrutura para guardar os modelos
std::vector<Model> models;

//File struct
typedef struct ficheiro{
    string name;
    GLuint size;
    GLuint indexn;
    GLuint indext;
    GLuint indexp;
} *File;

//Texture struct
typedef struct tex{
    string name;
    GLuint idtext;
}*Textura;

//Texturas
vector<Textura> texturas;

//Lights
vector<Light*> lights;

//Files
vector<File> Vbos;

int loadindTexture(std::string s) {
    unsigned int t,tw,th;
    unsigned char *texData;
    GLuint texID;

    ilInit();
    ilEnable(IL_ORIGIN_SET);

    ilGenImages(1,&t);
    ilBindImage(t);
    ilLoadImage((ILstring)s.c_str());
    tw = ilGetInteger(IL_IMAGE_WIDTH);
    th = ilGetInteger(IL_IMAGE_HEIGHT);
    ilConvertImage(IL_RGBA, IL_UNSIGNED_BYTE);
    texData = ilGetData();

    glGenTextures(1,&texID);

    glBindTexture(GL_TEXTURE_2D,texID);
    glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_S,		GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_WRAP_T,		GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER,   	GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tw, th, 0, GL_RGBA, GL_UNSIGNED_BYTE, texData);
    glGenerateMipmap(GL_TEXTURE_2D);

    return texID;
}


//Função que lê os ficheiros e desenha os vértices
File readFile(string file)
{
    
	// Variáveis
	float x, y, z;
    float tx, ty;
    float nx, ny, nz; 

	string linha;
	ifstream f("../3d/" + file);

    File vbo = new struct ficheiro;
    vbo->name = file;
    vector<float> vertexp;
    vector<float> vertexn;
    vector<float> vertext;

    vertexp.clear();
    vertexn.clear();
    vertext.clear();

    // Abre o ficheiro a ser lido
    while (std::getline(f, linha))
    {
        // Lê os pontos linha a linha e adiciona os vértices
        istringstream in(linha);
        in >> x;
        in >> y;
        in >> z;
        in >> nx;
        in >> ny;
        in >> nz;
        in >> tx;
        in >> ty;        

        vertexp.push_back(x);
        vertexp.push_back(y);
        vertexp.push_back(z);

        vertexn.push_back(nx);
        vertexn.push_back(ny);
        vertexn.push_back(nz);

        vertext.push_back(tx);
        vertext.push_back(ty);
	}

    vbo->size = vertexp.size()/3;
    Vbos.push_back(vbo);

    //Criar vbo
    glGenBuffers(1, &(vbo->indexp));
    glBindBuffer(GL_ARRAY_BUFFER, vbo->indexp);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexp.size(), vertexp.data(), GL_STATIC_DRAW);

    //criar vbo
    glGenBuffers(1,&(vbo->indexn));//copiar vbo para a grafica
    glBindBuffer(GL_ARRAY_BUFFER,vbo->indexn);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertexn.size(), vertexn.data(),GL_STATIC_DRAW);

    //criar vbo
    glGenBuffers(1,&(vbo->indext));
    glBindBuffer(GL_ARRAY_BUFFER,vbo->indext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertext.size(), vertext.data(),GL_STATIC_DRAW);    

    f.close();

    return vbo;
}


void lightsParser(XMLElement *grupo){
    XMLElement* light = grupo->FirstChildElement();
    GLuint lightIndex = 0;
    while (light != NULL) {
        if (strcmp(light->Attribute("type"),"point")==0) {
            float x = atof(light->Attribute("posX"));
            float y = atof(light->Attribute("posY"));
            float z = atof(light->Attribute("posZ"));

            lights.push_back(new LightPoint(x, y, z, lightIndex));
        } else if (strcmp(light->Attribute("type"),"directional")==0) {
            float dirx = atof(light->Attribute("dirX"));
            float diry = atof(light->Attribute("dirY"));
            float dirz = atof(light->Attribute("dirZ"));

            lights.push_back(new LightDirectional(dirx, diry, dirz, lightIndex));
        } else if (strcmp(light->Attribute("type"),"spotlight")==0) {
            float x = atof(light->Attribute("posX"));
            float y = atof(light->Attribute("posY"));
            float z = atof(light->Attribute("posZ"));
            float dirx = atof(light->Attribute("dirX"));
            float diry = atof(light->Attribute("dirY"));
            float dirz = atof(light->Attribute("dirZ"));
            float cutoff = atof(light->Attribute("cutoff"));

            lights.push_back(new LightSpotlight(x, y, z, dirx, diry, dirz, cutoff, lightIndex));
        }
        
        lightIndex++;
        light = light->NextSiblingElement();
    }
}

//Função para parse dos grupos
void groupParser(XMLElement *pGroup, vector<Group*> group) {
    //Variáveis
    float x,y,z,angle,time = 0.0f;
    bool align = false;
    string file = "";
    XMLElement *pSubGroup = pGroup->FirstChildElement();
    
    while(pSubGroup != NULL) {
        if(strcmp(pSubGroup->Value(),"transform") == 0) {
            XMLElement *pTransform = pSubGroup->FirstChildElement();
            while(pTransform != NULL) {
                //Procurar translação
                if(strcmp(pTransform->Value(),"translate") == 0) {
                    //Contar o número de pontos >= 4
                    int number = 0;
                    //Parse nos pontos da curva 
                    XMLElement *pPoint = pTransform->FirstChildElement();
                    vector<float> vertices;
                    if(pPoint != NULL) {
                        while(pPoint != NULL) {
                            //Guardar os pontos
                            x = float(pPoint->FindAttribute("x")->FloatValue());
                            y = float(pPoint->FindAttribute("y")->FloatValue());
                            z = float(pPoint->FindAttribute("z")->FloatValue());
                            vertices.push_back(x);
                            vertices.push_back(y);
                            vertices.push_back(z);
                            pPoint = pPoint->NextSiblingElement(); 
                        }
                        time = float(pTransform->FindAttribute("time")->FloatValue());
                        align = float(pTransform->FindAttribute("align")->BoolValue());
                    } else {
                        x = float(pTransform->FindAttribute("x")->FloatValue());
                        y = float(pTransform->FindAttribute("y")->FloatValue());
                        z = float(pTransform->FindAttribute("z")->FloatValue());
                        time = 0.0f;
                        align = false;
                    }
                    //Criar árvore auxiliar e guardar translação 
                    group.push_back(new Translate(x,y,z,time,align,vertices));
                }
                 //Procurar rotação
                if(strcmp(pTransform->Value(),"rotate") == 0) {
                    if(pTransform->Attribute("angle")) {
                        angle = float(pTransform->FindAttribute("angle")->FloatValue());
                    }
                    if(pTransform->Attribute("time")) {
                        time = float(pTransform->FindAttribute("time")->FloatValue());
                    }
                    x = float(pTransform->FindAttribute("x")->FloatValue());
                    y = float(pTransform->FindAttribute("y")->FloatValue());
                    z = float(pTransform->FindAttribute("z")->FloatValue());
                    //Criar árvore auxiliar e guardar a rotação
                    group.push_back(new Rotate(x,y,z,angle,time));
                }
                //Procurar escala
                if(strcmp(pTransform->Value(),"scale") == 0) {
                    x = float(pTransform->FindAttribute("x")->FloatValue());
                    y = float(pTransform->FindAttribute("y")->FloatValue());
                    z = float(pTransform->FindAttribute("z")->FloatValue());
                    //Criar árvore auxiliar e guardar a rotação
                    group.push_back(new Scale(x,y,z));
                }
                pTransform = pTransform->NextSiblingElement();
            }
        }
        //Procurar os ficheiros (pColor)

        if(strcmp(pSubGroup->Value(),"models") == 0){
            //Variáveis auiliares
            GLuint idTex = -1;
            string file = "";
            string texture = "";
            vector<Color*> color;
            float r,g,b,s;

            XMLElement* pModel = pSubGroup->FirstChildElement("model");
            while(pModel != NULL){

                if (strcmp(pModel->Value(),"model") == 0){
                    const char * model = pModel->FindAttribute("file")->Value();
                    file = model;
                    int flag = 0;
                    File aux;

                    for(File vbo : Vbos){
                        if(strcmp(model,vbo->name.c_str()) == 0) {
                            flag = 1;
                            aux = vbo;
                            break;
                        }
                    }

                    if (flag == 0){
                        aux = readFile(file.c_str());
                        Vbos.push_back(aux);
                    }
                }

                if(strcmp(pModel->Value(), "texture") == 0){
                    int find = 0;
                    Textura aux;
                    texture = pModel->Attribute("file");
                    for(Textura t : texturas){
                        if(strcmp(texture.c_str(),t->name.c_str()) == 0){
                            find = 1;
                            aux = t;
                            break;
                        }
                    }
                    if(find == 0){
                        aux = new struct tex;
                        aux->idtext = loadindTexture("textures/" + texture);
                        aux->name = texture;
                        texturas.push_back(aux);
                    }
                    idTex = aux->idtext;
                }
                

                if(strcmp(pModel->Value(), "color") == 0){
                    XMLElement* pColor = pModel->FirstChildElement();

                    while (pColor != NULL) {
                        if(strcmp(pColor->Value(),"diffuse") == 0) {
                            if (pColor->Attribute("R")) {
                                pColor->QueryFloatAttribute("R", &r);
                            }
                            if (pColor->Attribute("G")) {
                                pColor->QueryFloatAttribute("G", &g);
                            }
                            if (pColor->Attribute("B")) {
                                pColor->QueryFloatAttribute("B", &b);
                            }
                            color.push_back(new Diffuse(r, g, b));
                        }
                        if(strcmp(pColor->Value(),"ambient") == 0) {
                            if (pColor->Attribute("R")) {
                                pColor->QueryFloatAttribute("R", &r);
                            }
                            if (pColor->Attribute("G")) {
                                pColor->QueryFloatAttribute("G", &g);
                            }
                            if (pColor->Attribute("B")) {
                                pColor->QueryFloatAttribute("B", &b);
                            }
                            color.push_back(new Ambient(r, g, b));
                        }
                        if(strcmp(pColor->Value(),"specular") == 0) {
                            if (pColor->Attribute("R")) {
                                pColor->QueryFloatAttribute("R", &r);
                            }
                            if (pColor->Attribute("G")) {
                                pColor->QueryFloatAttribute("G", &g);
                            }
                            if (pColor->Attribute("B")) {
                                pColor->QueryFloatAttribute("B", &b);
                            }
                            color.push_back(new Specular(r, g, b));
                        }
                        if(strcmp(pColor->Value(),"emissive") == 0) {
                             if (pColor->Attribute("R")) {
                                pColor->QueryFloatAttribute("R", &r);
                            }
                            if (pColor->Attribute("G")) {
                                pColor->QueryFloatAttribute("G", &g);
                            }
                            if (pColor->Attribute("B")) {
                                pColor->QueryFloatAttribute("B", &b);
                            }
                            color.push_back(new Emissive(r, g, b));
                        }
                        if(strcmp(pColor->Value(),"shininess") == 0) {
                            if (pColor->Attribute("value")) {
                                pColor->QueryFloatAttribute("value", &s);
                            }
                            color.push_back(new Shininess(s));
                        }
                        pColor = pColor->NextSiblingElement();
                    }
                }
                pModel = pModel->NextSiblingElement();
                if(pModel == NULL) {
                    models.push_back(Model(file,group,color,idTex));
                }
            }
        }
        //Tratar dos subgroupos
        if(strcmp(pSubGroup->Value(),"group") == 0){
            //inicializar a subarvore do grupo para ser adicionada na arvore de classes
            groupParser(pSubGroup, group);
        }
        pSubGroup = pSubGroup->NextSiblingElement();
    }
}

//Função que lê o ficheiro com a config
int readXML()
{

	// Abre File
	XMLDocument config;
	XMLError eResult = config.LoadFile("config.xml");

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

    XMLElement *pSubGroup = pRootElement->FirstChildElement("lights");

    if(strcmp(pSubGroup->Value(),"lights") == 0){
        lightsParser(pSubGroup);
    }

    // procura o elemento group
    XMLElement *pGroup = pRootElement->FirstChildElement("group");

    //Percorre os grupos
    while (pGroup != NULL) {
        vector<Group*> group;
        groupParser(pGroup, group);
        pGroup = pGroup->NextSiblingElement();
    }
	return 1;
}

//Função que lê a estrutura de dados e desenha a cena
int draw() {
    for(Model m : models) {
        File aux;
        for(File v : Vbos) {
            if(m.model==v->name) {
                aux = v;
                break;
            }
        }

        glPushMatrix();
        for (Group* t : m.transformations) {
            t->apply();
        }
        if(m.colors.size() != 0) {
            for (Color* c : m.colors) {
                c->apply();
            }
        } else {
            float diff[4] = {200.0f, 200.0f, 200.0f, 1.0f};
            float spec[4] = {50.0f, 50.0f, 50.0f, 1.0f};
            float amb[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            float emi[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            glMaterialfv(GL_FRONT, GL_AMBIENT, amb);
            glMaterialfv(GL_FRONT, GL_SPECULAR, spec);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
            glMaterialfv(GL_FRONT, GL_EMISSION, emi);
            glMaterialf(GL_FRONT, GL_SHININESS, 128.0f);
        }

        glBindBuffer(GL_ARRAY_BUFFER, aux->indexp);
        glVertexPointer(3, GL_FLOAT, 0, 0);
        
        glBindBuffer(GL_ARRAY_BUFFER, aux->indexn);
        glNormalPointer(GL_FLOAT, 0, 0);
        
        if (m.texture != -1) {
            glBindTexture(GL_TEXTURE_2D, m.texture);

            glBindBuffer(GL_ARRAY_BUFFER, aux->indext);
            glTexCoordPointer(2, GL_FLOAT, 0, 0);
        }
        glDrawArrays(GL_TRIANGLES, 0, aux->size);

        float clear[4] = {0.0f, 0.0f, 0.0f, 1.0f};
        glMaterialfv(GL_FRONT, GL_EMISSION, clear);
        glBindTexture(GL_TEXTURE_2D, 0);
        glPopMatrix();
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

void drawAxis() {
    glDisable(GL_LIGHTING);

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

    glEnable(GL_LIGHTING);
}

void initLights(){

    float dark[4] = {0.3, 0.3, 0.3, 1.0};
    float white[4] = {1.0, 1.0, 1.0, 1.0};
    // float black[4] = {0.0f, 0.0f, 0.0f, 0.0f};

    for (Light* l: lights) {
        glEnable(l->index);
        
        // light colors
        glLightfv(l->index, GL_AMBIENT, dark);
        glLightfv(l->index, GL_DIFFUSE, white);
        glLightfv(l->index, GL_SPECULAR, white);

    }

    for (Light* l: lights) {
        l->apply();
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

	// put axis drawing in here
    drawAxis();
	// put the geometric transformations here

    initLights(); 

    glColor3f(1,1,1);
    draw();
	// End of frame
	glutSwapBuffers();
}


void initGL(){
    glEnable(GL_LIGHTING);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_TEXTURE_2D);
}

int main(int argc, char **argv)
{

    glutGet(GLUT_ELAPSED_TIME);
	// Init GLUT and the window 
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(wWidth, wHeight);
	window = glutCreateWindow("CG@DI-UM");
    
	// Required callback registry
    initGL();

	glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

    glewInit();
    ilInit();

    readXML();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

	// put here the registration of the keyboard callbacks
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processNormalKeys);
    glutPassiveMotionFunc(processMouse);
    
	// enter GLUT's main cycle
	glutMainLoop();
}
