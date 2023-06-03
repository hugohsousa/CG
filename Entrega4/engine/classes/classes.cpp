#include <tuple>
#include <vector>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <stdlib.h>
#include <GL/glut.h>
#endif

#include  "../headers/classes.h"
#include "../headers/catmull.h"
#include <string>
using namespace std;

Group :: Group(int id) { setId(id); }
Group :: Group() { id = 0; }
void Group :: setId(int n) { id = n; }
int Group :: getId() { return id; }
void Group :: apply() { glPushMatrix(); }

Model :: Model() { file = ""; }
Model :: Model(string file){ setFile(file); }
void Model :: setFile(string f) { file = f; }
string Model :: getFile() { return file; }
void Model :: apply() {};

Texture :: Texture() { file = ""; }
Texture :: Texture(string file) { setFile(file); }
void Texture :: setFile(string f) { file = f; }
string Texture :: getFile() { return file; }
void Texture :: apply() {};

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
