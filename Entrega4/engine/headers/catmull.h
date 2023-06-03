#ifndef ENGINE_CATMULL_H
#define ENGINE_CATMULL_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <stdlib.h>
#include <GL/glut.h>
#endif

#include <string>
#include <vector>

using namespace std;

void buildRotMatrix(float *x, float *y, float *z, float *m);
void cross(float *a, float *b, float *res);
void normalize(float *a);
float length(float *v);
void multMatrixVector(float *m, float *v, float *res);
void getCatmullRomPoint(float t, vector<vector<float>>vecP,  float* pos, float* deriv);
void getGlobalCatmullRomPoint(float gt, float* pos, float* deriv, vector<vector<float>> pontosControlo);
void renderCatmullRomCurve(vector<vector<float>> PontosControlo);
void drawOrbita(vector<float> vertex);

#endif //ENGINE_CATMULL_H
