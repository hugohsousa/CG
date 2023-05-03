#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <stdlib.h>
#include <GL/glut.h>
#endif

#define _USE_MATH_DEFINES
#include <cmath>
#include <vector>
#include <iostream>
using namespace std;

//Catmull
void buildRotMatrix(float *x, float *y, float *z, float *m) {

    m[0] = x[0]; m[1] = x[1]; m[2] = x[2]; m[3] = 0;
    m[4] = y[0]; m[5] = y[1]; m[6] = y[2]; m[7] = 0;
    m[8] = z[0]; m[9] = z[1]; m[10] = z[2]; m[11] = 0;
    m[12] = 0; m[13] = 0; m[14] = 0; m[15] = 1;
}


void cross(float *a, float *b, float *res) {

    res[0] = a[1]*b[2] - a[2]*b[1];
    res[1] = a[2]*b[0] - a[0]*b[2];
    res[2] = a[0]*b[1] - a[1]*b[0];
}


void normalize(float *a) {
    float l = sqrt(a[0]*a[0] + a[1] * a[1] + a[2] * a[2]);
    a[0] = a[0]/l;
    a[1] = a[1]/l;
    a[2] = a[2]/l;
}


float length(float *v) {

    float res = sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
    return res;

}

void multMatrixVector(float *m, float *v, float *res) {

    for (int j = 0; j < 4; ++j) {
        res[j] = 0;
        for (int k = 0; k < 4; ++k) {
            res[j] += v[k] * m[j * 4 + k];
        }
    }
}

//t = tessellation , vecP = vector de pontos de translacao
void getCatmullRomPoint(float t, vector<vector<float>>vecP,  float* pos, float* deriv) {


    // catmull-rom matrix
    float m[4][4] = {{-0.5f, 1.5f,  -1.5f, 0.5f},
                     {1.0f,  -2.5f, 2.0f,  -0.5f},
                     {-0.5f, 0.0f,  0.5f,  0.0f},
                     {0.0f,  1.0f,  0.0f,  0.0f}};

    float T[4] = {t * t * t, t * t, t, 1};
    float Td[4] = {3 * t * t, 2 * t, 1, 0};
    float A[4][4];

    // Compute A = M * P
    // compute pos = T * A;
    // compute pos = T' * A;
    for (int i = 0; i < 3; i++) {
        float p[4] = {vecP[0][i], vecP[1][i], vecP[2][i], vecP[3][i]};
        multMatrixVector((float *) m, p, A[i]);
        pos[i] = A[i][0] * T[0] + A[i][1] * T[1] + A[i][2] * T[2] + A[i][3] * T[3];
        deriv[i] = A[i][0] * Td[0] + A[i][1] * Td[1] + A[i][2] * Td[2] + A[i][3] * Td[3];
    }
}


// given  global t, returns the Point in the curve
void getGlobalCatmullRomPoint(float gt, float* pos, float* deriv, vector<vector<float>> pontosControlo) {

    int nPontos = pontosControlo.size();

    float t = gt * nPontos; // this is the real global t
    int index = floor(t);  // which segment
    t = t - index; // where within  the segment

    // indices store the points
    int indices[4];
    indices[0] = (index + nPontos-1)%nPontos;
    indices[1] = (indices[0]+1)%nPontos;
    indices[2] = (indices[1]+1)%nPontos;
    indices[3] = (indices[2]+1)%nPontos;

    vector <vector<float>> pvec = {pontosControlo[indices[0]], pontosControlo[indices[1]], pontosControlo[indices[2]], pontosControlo[indices[3]]};

    getCatmullRomPoint(t,pvec, pos, deriv);
}

void renderCatmullRomCurve(vector<vector<float>>PontosControlo) {

// draw curve using line segments with GL_LINE_LOOP
    float pos[4];
    float deriv[4];

    float gt = 0;

    glBegin(GL_LINE_LOOP);
        for (int i = 0; i < 100; i++) {
            getGlobalCatmullRomPoint(gt, pos, deriv, PontosControlo);
            glVertex3f(pos[0],pos[1],pos[2]);
            gt += 0.01;
        }
    glEnd();
}

void drawOrbita(vector<float> vertex){

    glBegin(GL_LINE_LOOP);
    for(int i = 0;i<vertex.size();i+=3){
        glVertex3f(vertex.at(i),vertex.at(i+1),vertex.at(i+2));
    }
    glEnd();
}

