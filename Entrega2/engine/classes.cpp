#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <stdlib.h>
#include <GL/glut.h>
#endif

#include  "classes.h"
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

Translate :: Translate() { x = y = z = 0.0f;}
Translate :: Translate(float x, float y, float z) { setX(x); setY(y); setZ(z); }
void Translate :: setX(float n) { x = n; }
void Translate :: setY(float n) { y = n; }
void Translate :: setZ(float n) { z = n; }
float Translate :: getX() { return x; }
float Translate :: getY() { return y; }
float Translate :: getZ() { return z; }
void Translate :: apply() { glTranslatef(x,y,z); }

Rotate :: Rotate() { x = y = z = angle =0.0f;}
Rotate :: Rotate(float x, float y, float z, float angle) { setX(x); setY(y); setZ(z); setAngle(angle);}
void Rotate :: setX(float n){ x = n; }
void Rotate :: setY(float n){ y = n; }
void Rotate :: setZ(float n){ z = n; }
void Rotate :: setAngle(int n) { angle = n; }
float Rotate :: getX() { return x; }
float Rotate :: getY() { return y; }
float Rotate :: getZ() { return z; }
float Rotate :: getAngle() { return angle; }
void Rotate :: apply() { glRotatef(angle,x,y,z); }

Scale :: Scale() { x = y = z = 0.0f;}
Scale :: Scale(float x, float y, float z) { setX(x); setY(y); setZ(z); }
void Scale :: setX(float n) { x = n; }
void Scale :: setY(float n) { y = n; }
void Scale :: setZ(float n) { z = n; }
float Scale :: getX() { return x; }
float Scale :: getY() { return y; }
float Scale :: getZ() { return z; }
void Scale :: apply() { glScalef(x,y,z); }
