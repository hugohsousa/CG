#include <tuple>
#include <vector>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <stdlib.h>
#include <GL/glew.h>
#include <GL/glut.h>
#include <IL/il.h>
#endif

#include  "../headers/classes.h"
#include "../headers/catmull.h"
#include <string>
using namespace std;


Translate :: Translate() { 
    x = y = z = time = 0.0f;
    curve = vector<float>();
    align = false;
}

Translate :: Translate(float x, float y, float z,float time, bool align, vector<float> curve) {
    setX(x); 
    setY(y);
    setZ(z);
    setTime(time);
    setAlign(align);
    setCurve(curve);
}
void Translate :: setX(float n) { x = n; }
void Translate :: setY(float n) { y = n; }
void Translate :: setZ(float n) { z = n; }
void Translate :: setTime(float n) { time = n; }
void Translate :: setAlign(bool n) { align = n; }
void Translate :: setCurve(vector<float> n) { curve = n; }
float Translate :: getX() { return x; }
float Translate :: getY() { return y; }
float Translate :: getZ() { return z; }
float Translate :: getTime() { return time; }
bool Translate :: getAlign() { return align; }
vector<vector<float>> Translate :: getCurve(){

    vector<vector<float>> curva;

    for(int i = 0 ; i < (curve.size()/3) ;i++  ){ //i entre 0 e 4
        vector<float> vaux;
        for (int j = 0; j <3 ; j++){ //j entre 0 e 3
            float aux = curve.at((i*3)+j);
            vaux.push_back(aux);
        }
        curva.push_back(vaux);
        vaux.clear();
    }
    return curva;
}
void Translate :: apply() { 
   float time = (glutGet(GLUT_ELAPSED_TIME))/((float) 1000);

    if (getTime() != 0.0f) {

        float pos[4];
        float deriv[4];
        float X[4];
        float Y[4] = {0,1,0};
        float Z[4];

        glDisable(GL_LIGHTING);

        float gt = ((getCurve().size())+time)/getTime();
        getGlobalCatmullRomPoint(gt, pos, deriv, getCurve());
        renderCatmullRomCurve(getCurve());
        //cout  << pos[0]  << pos[1]<<pos[2] << endl;

        glTranslatef(pos[0],pos[1],pos[2]);

        X[0] = deriv[0]; X[1] = deriv[1]; X[2] = deriv[2]; X[3] = deriv[3];
        normalize(X);
        cross(X, Y, Z);

        // normalize ZZ and get YY
        normalize(Z);
        cross(Z, X, Y);

        // normalize YY and up = YY
        normalize(Y);
        //memcpy(up, Y, 3 * sizeof(float));

        // build rotation matrix
        float m[4][4];
        buildRotMatrix(X, Y, Z, (float*)m);
        glMultMatrixf((float*)m); 
        glEnable(GL_LIGHTING);
    }
    else {
        glTranslatef(x,y,z); 
    }
}

Rotate :: Rotate() { x = y = z = angle = time = 0.0f;}
Rotate :: Rotate(float x, float y, float z, float angle, float time) { 
    setX(x);
    setY(y);
    setZ(z);
    setAngle(angle);
    setTime(time);
}
void Rotate :: setX(float n){ x = n; }
void Rotate :: setY(float n){ y = n; }
void Rotate :: setZ(float n){ z = n; }
void Rotate :: setAngle(int n) { angle = n; }
void Rotate :: setTime(int n) { time = n; }
float Rotate :: getX() { return x; }
float Rotate :: getY() { return y; }
float Rotate :: getZ() { return z; }
float Rotate :: getAngle() { return angle; }
float Rotate :: getTime() { return time; }
void Rotate :: apply() { 
    if(time == 0.0) {
        glRotatef(angle,x,y,z);
    }
    else{
        float t = (glutGet(GLUT_ELAPSED_TIME))/((float) 1000);
        glRotatef((t/time)*360,x,y,z);
    }
}
Scale :: Scale() { x = y = z = 0.0f;}
Scale :: Scale(float x, float y, float z) { setX(x); setY(y); setZ(z); }
void Scale :: setX(float n) { x = n; }
void Scale :: setY(float n) { y = n; }
void Scale :: setZ(float n) { z = n; }
float Scale :: getX() { return x; }
float Scale :: getY() { return y; }
float Scale :: getZ() { return z; }
void Scale :: apply() { glScalef(x,y,z); }


Diffuse::Diffuse(float r, float g, float b) {
    this->rgb[0] = r;
    this->rgb[1] = g;
    this->rgb[2] = b;
    this->rgb[3] = 1.0f;
}

void Diffuse::apply() {
    glMaterialfv(GL_FRONT, GL_DIFFUSE, this->rgb);
}

Ambient::Ambient(float r, float g, float b) {
    this->rgb[0] = r;
    this->rgb[1] = g;
    this->rgb[2] = b;
    this->rgb[3] = 1.0f;
}

void Ambient::apply() {
    glMaterialfv(GL_FRONT, GL_AMBIENT, this->rgb);
}

Specular::Specular(float r, float g, float b) {
    this->rgb[0] = r;
    this->rgb[1] = g;
    this->rgb[2] = b;
    this->rgb[3] = 1.0f;
}

void Specular::apply() {
    glMaterialfv(GL_FRONT, GL_SPECULAR, this->rgb);
}

Emissive::Emissive(float r, float g, float b) {
    this->rgb[0] = r;
    this->rgb[1] = g;
    this->rgb[2] = b;
    this->rgb[3] = 1.0f;
}

void Emissive::apply() {
    glMaterialfv(GL_FRONT, GL_EMISSION, this->rgb);
}

Shininess::Shininess(float r) {
    this->s = r;
}

void Shininess::apply() {
    glMaterialf(GL_FRONT, GL_SHININESS, this->s);
}

Model::Model(string model, vector<Group*> t, vector<Color*> b, GLuint texture) {
    this->model = model;
    this->texture = texture;
    this->transformations = t;
    this->colors = b;
}

Model::Model() {}

LightPoint::LightPoint(float a, float b, float c, int i) {
    this->pos[0] = a;
    this->pos[1] = b;
    this->pos[2] = c;
    this->pos[3] = 1.0f;
    this->index = getLight(i);
}

void LightPoint::apply() {
    glLightfv(this->index, GL_POSITION, this->pos);
}

LightDirectional::LightDirectional(float a, float b, float c, int i) {
    this->dir[0] = a;
    this->dir[1] = b;
    this->dir[2] = c;
    this->dir[3] = 0.0f;
    this->index = getLight(i);
}

void LightDirectional::apply() {
    glLightfv(this->index, GL_POSITION, this->dir);
}

LightSpotlight::LightSpotlight(float a, float b, float c, float da, float db, float dc, GLfloat ct, int i) {
    this->pos[0] = a;
    this->pos[1] = b;
    this->pos[2] = c;
    this->pos[3] = 1.0f;
    this->dir[0] = da;
    this->dir[1] = db;
    this->dir[2] = dc;
    this->dir[3] = 0.0f;
    this->cutoff = ct;
    this->index = getLight(i);
}

void LightSpotlight::apply() {
    glLightfv(this->index, GL_POSITION, this->pos);
    glLightfv(this->index, GL_SPOT_DIRECTION, this->dir);
    glLightfv(this->index, GL_SPOT_CUTOFF, &(this->cutoff));
}

int getLight(int nLight) {
    int CLight;
    switch (nLight) {
        case 0: CLight = GL_LIGHT0; break;
        case 1: CLight = GL_LIGHT1; break;
        case 2: CLight = GL_LIGHT2; break;
        case 3: CLight = GL_LIGHT3; break;
        case 4: CLight = GL_LIGHT4; break;
        case 5: CLight = GL_LIGHT5; break;
        case 6: CLight = GL_LIGHT6; break;
        case 7: CLight = GL_LIGHT7; break;
        default: exit(1);
    }
    return CLight;
}

