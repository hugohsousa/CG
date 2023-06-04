#include <IL/il.h>
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

// Estrutura para guardar os grupos
typedef struct node{
    Group* g;
    string label;
    vector<struct node*> next;
} *Tree;

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

//Light struct
typedef struct luz{
    string type;
    float pos[4];
    float dir[4];
    float diff[4];
    float amb[4];
    float spec[4];
    float cutoff;
    float exp;
    float quad;
    float lin;
} *Light;

//Texturas
vector<Textura> texturas;

//Lights
vector<Light> lights;

//Files
vector<File> Vbos;
Tree groupTree;
int n_group = 0;

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
    printf("teste:%f\n",vertexn.data()[1]);
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
    while (light != nullptr) {
        Light luz = new struct luz;
        luz->pos[3] = 1.0f;
        luz->dir[3] = 0.0f;
        luz->diff[0] = 1.0f;
        luz->diff[1] = 1.0f;
        luz->diff[2] = 1.0f;
        luz->diff[3] = 1.0f;
        luz->amb[0] = 0.2f;
        luz->amb[1] = 0.2f;
        luz->amb[2] = 0.2f;
        luz->amb[3] = 1.0f;
        luz->spec[0] = 1.0f;
        luz->spec[1] = 1.0f;
        luz->spec[2] = 1.0f;
        luz->spec[3] = 1.0f;

        //Modelo
        if (light->Attribute("type")) {
            luz->type = light->Attribute("type");
        }
        if (light->Attribute("diffR")) {
            light->QueryFloatAttribute("diffR", &(luz->diff[0]));
        }
        if (light->Attribute("diffG")) {
            light->QueryFloatAttribute("diffG", &(luz->diff[1]));
        }
        if (light->Attribute("diffB")) {
            light->QueryFloatAttribute("diffB", &(luz->diff[2]));
        }
        if (light->Attribute("specR")) {
            light->QueryFloatAttribute("specR", &(luz->spec[0]));
        }
        if (light->Attribute("specG")) {
            light->QueryFloatAttribute("specG", &(luz->spec[1]));
        }
        if (light->Attribute("specB")) {
            light->QueryFloatAttribute("specB", &(luz->spec[2]));
        }
        if (light->Attribute("ambR")) {
            light->QueryFloatAttribute("ambiR",&(luz->amb[0]));
        }
        if (light->Attribute("ambG")) {
            light->QueryFloatAttribute("ambiG", &(luz->amb[1]));
        }
        if (light->Attribute("ambB")) {
            light->QueryFloatAttribute("ambiB", &(luz->amb[2]));
        }
        if (light->Attribute("posX")) {
            light->QueryFloatAttribute("posX",&(luz->pos[0]));
        }
        if (light->Attribute("posY")) {
            light->QueryFloatAttribute("posY", &(luz->pos[1]));
        }
        if (light->Attribute("posZ")) {
            light->QueryFloatAttribute("posZ", &(luz->pos[2]));
        }
        if (light->Attribute("dirX")) {
            light->QueryFloatAttribute("dirX",&(luz->dir[0]));
        }
        if (light->Attribute("dirY")) {
            light->QueryFloatAttribute("dirY", &(luz->dir[1]));
        }
        if (light->Attribute("dirZ")) {
            light->QueryFloatAttribute("dirZ", &(luz->dir[2]));
        }
        if (light->Attribute("cutoff")) {
            light->QueryFloatAttribute("cutoff", &(luz->cutoff));
        }
        if (light->Attribute("exp")) {
            light->QueryFloatAttribute("exp", &(luz->exp));
        }
        if (light->Attribute("lin")) {
            light->QueryFloatAttribute("lin", &(luz->lin));
        }
        if (light->Attribute("quad")) {
            light->QueryFloatAttribute("quad", &(luz->quad));
        }

        lights.push_back(luz);

        light = light->NextSiblingElement();
    }
}

//Função para parse dos grupos
void groupParser(XMLElement *pGroup, Tree group) {
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
                    Tree aux = new struct node;
                    aux->g = new Translate(x,y,z,time,align,vertices);
                    aux->label = "translate";
                    aux->next.clear();
                    group->next.push_back(aux);
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
        //Procurar os ficheiros (pColor)

        if(strcmp(pSubGroup->Value(),"models") == 0){
            XMLElement* pModel = pSubGroup->FirstChildElement("model");
            while(pModel != NULL){
                //Variáveis auiliares
                GLuint idTex = -1;
                string file = "";
                string texture = "";
                vector<float> color;
                for(int j = 0; j <= 12; j++){
                    color.push_back(-1.0f);
                }
                color[0] = 200.0f;color[1] = 200.0f;color[2] = 200.0f;
                color[3] = 50.0f;color[4] = 50.0f;color[5] = 50.0f;
                color[6] = 0.0f;color[7] = 0.0f;color[8] = 0.0f;
                color[9] = 0.0f;color[10] = 0.0f;color[11] = 0.0f;
                color[12] = 0.0f;

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
                        printf("sajdlsad:%s\n",file.c_str());
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
                                pColor->QueryFloatAttribute("R", &color[0]);
                            }
                            if (pColor->Attribute("G")) {
                                pColor->QueryFloatAttribute("G", &color[1]);
                            }
                            if (pColor->Attribute("B")) {
                                pColor->QueryFloatAttribute("B", &color[2]);
                            }
                        }
                        if(strcmp(pColor->Value(),"ambient") == 0) {
                            if (pColor->Attribute("R")) {
                                pColor->QueryFloatAttribute("R", &color[3]);
                            }
                            if (pColor->Attribute("G")) {
                                pColor->QueryFloatAttribute("G", &color[4]);
                            }
                            if (pColor->Attribute("B")) {
                                pColor->QueryFloatAttribute("B", &color[5]);
                            }
                        }
                        if(strcmp(pColor->Value(),"specular") == 0) {
                            if (pColor->Attribute("R")) {
                                pColor->QueryFloatAttribute("R", &color[6]);
                            }
                            if (pColor->Attribute("G")) {
                                pColor->QueryFloatAttribute("G", &color[7]);
                            }
                            if (pColor->Attribute("B")) {
                                pColor->QueryFloatAttribute("B", &color[8]);
                            }
                        }
                        if(strcmp(pColor->Value(),"emissive") == 0) {
                             if (pColor->Attribute("R")) {
                                pColor->QueryFloatAttribute("R", &color[9]);
                            }
                            if (pColor->Attribute("G")) {
                                pColor->QueryFloatAttribute("G", &color[10]);
                            }
                            if (pColor->Attribute("B")) {
                                pColor->QueryFloatAttribute("B", &color[11]);
                            }
                        }
                        if(strcmp(pColor->Value(),"shininess") == 0) {
                            if (pColor->Attribute("value")) {
                                pColor->QueryFloatAttribute("value", &color[12]);
                            }
                        }
                        pColor = pColor->NextSiblingElement();
                    }
                }
                pModel = pModel->NextSiblingElement();
                if(pModel == NULL) {
                    Tree aux = new struct node;
                    aux->g = new Model(file,idTex,color);
                    aux->label = "model";
                    aux->next.clear();
                    group->next.push_back(aux);
                }
            }
        }
        if(strcmp(pSubGroup->Value(),"group") == 0){
        lightsParser(pSubGroup);
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
                    //printf("Found vbo%d\n",aux->indext); 
                    flag = 1;
                    break;
                }
            }
            vector<float> cor = (dynamic_cast<Model*>(n->g))->getColors();
            float diff[4] = {cor[0],cor[1],cor[2],1.0f};
            float amb[4] = {cor[3],cor[4],cor[5],1.0f};
            float spec[4] = {cor[6],cor[7],cor[8],1.0f};
            float emi[4] = {cor[9],cor[10],cor[11],1.0f};
            glMaterialfv(GL_FRONT, GL_AMBIENT, spec);
            glMaterialfv(GL_FRONT, GL_SPECULAR, amb);
            glMaterialfv(GL_FRONT, GL_DIFFUSE, diff);
            glMaterialfv(GL_FRONT, GL_EMISSION, emi);
            glMaterialf(GL_FRONT, GL_SHININESS, cor[12]);
            glBindBuffer(GL_ARRAY_BUFFER,aux->indexp);
            glVertexPointer(3,GL_FLOAT,0,0);

            glBindBuffer(GL_ARRAY_BUFFER, aux->indexn);
            glNormalPointer(GL_FLOAT, 0, 0);

            GLuint idTex = (dynamic_cast<Model*>(n->g))->getTexture();
            if(idTex != -1) {
                glBindTexture(GL_TEXTURE_2D, idTex);
                glBindBuffer(GL_ARRAY_BUFFER,aux->indext);
                glTexCoordPointer(2,GL_FLOAT,0,0);            
            }
            glDrawArrays(GL_TRIANGLES, 0, aux->size);

            glBindTexture(GL_TEXTURE_2D, 0);
            float clear[4] = {0.0f, 0.0f, 0.0f, 1.0f};
            glMaterialfv(GL_FRONT, GL_EMISSION, clear);

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

void setLights() {
    for(int i = 0; i < fmin(8,lights.size()); i++){
        Light luz = lights[i];
        if(strcmp(luz->type.c_str(),"DIRECTIONAL") != 0) {
            glLightfv(GL_LIGHT0 + i, GL_POSITION, luz->pos);
        }
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


void renderScene(void)
{
    setLights();
    
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
	glColor3f(1,1,1);
    readTree(groupTree);
	// End of frame
	glutSwapBuffers();
}

void initLights(){
    for(int i = 0; i < fmin(8,lights.size()); i++){
        Light light = lights[i];
        glEnable(GL_LIGHT0+i);
        glLightfv(GL_LIGHT0 + i, GL_AMBIENT, light->amb);
        glLightfv(GL_LIGHT0 + i, GL_DIFFUSE, light->diff);
        glLightfv(GL_LIGHT0 + i, GL_SPECULAR, light->spec);

        if(strcmp((light->type).c_str(),"POINT") == 0){
            glLightf(GL_LIGHT0 + i,  GL_LINEAR_ATTENUATION, light->lin);
            glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, light->quad);
        }
        if(strcmp((light->type).c_str(),"DIRECTIONAL") == 0){
            glLightfv(GL_LIGHT0 + i,GL_SPOT_DIRECTION, light->dir);
        }
        if(strcmp((light->type).c_str(),"SPOT") == 0){
            glLightfv(GL_LIGHT0 + i, GL_SPOT_DIRECTION, light->dir);
            glLightf(GL_LIGHT0 + i, GL_SPOT_CUTOFF, light->cutoff);
            glLightf(GL_LIGHT0 + i,GL_SPOT_EXPONENT, light->exp);
            glLightf(GL_LIGHT0 + i, GL_LINEAR_ATTENUATION, light->lin);
            glLightf(GL_LIGHT0 + i, GL_QUADRATIC_ATTENUATION, light->quad);
        }
    }
}

void initGL(){

    readXML();
    
    glEnable(GL_LIGHTING);
    glEnable(GL_RESCALE_NORMAL);
    glEnable(GL_TEXTURE_2D);

    initLights();

    glEnableClientState(GL_VERTEX_ARRAY);
    glEnableClientState(GL_NORMAL_ARRAY);
    glEnableClientState(GL_TEXTURE_COORD_ARRAY);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
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
	glutDisplayFunc(renderScene);
    glutIdleFunc(renderScene);
	glutReshapeFunc(changeSize);

    glewInit();
    ilInit();
    initGL();

	// put here the registration of the keyboard callbacks
	glutSpecialFunc(processSpecialKeys);
	glutKeyboardFunc(processNormalKeys);
    glutPassiveMotionFunc(processMouse);
    
	// enter GLUT's main cycle
	glutMainLoop();
}
