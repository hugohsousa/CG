#ifndef MAIN_CPP_CLASSES_H
#define MAIN_CPP_CLASSES_H
#include <GL/gl.h>
#endif //MAIN_CPP_CLASSES_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <stdlib.h>
#include <GL/glut.h>
#endif

#include "catmull.h"
#include <string>
#include <vector>
using namespace std;

class Group {
    public:
    void virtual apply() = 0;
};


class Translate : public Group {
    private:
        float x,y,z,time;
        bool align;
        vector<float> curve;
    public:
        Translate();
        Translate(float x, float y, float z,float time,bool align,vector<float> curve);
        void setX(float n);
        void setY(float n);
        void setZ(float n);
        void setTime(float n);
        void setAlign(bool n);
        void setCurve(vector<float> n);
        float getX();
        float getY();
        float getZ();
        float getTime();
        bool getAlign();
        vector<vector<float>> getCurve();
        void apply();
};

class Rotate : public Group {
    private:
        float x,y,z,angle,time;
    public:
        Rotate();
        Rotate(float x, float y, float z, float angle,float time);
        void setX(float n);
        void setY(float n);
        void setZ(float n);
        void setAngle(int n);
        void setTime(int n);
        float getX();
        float getY();
        float getZ();
        float getAngle();
        float getTime();
       void apply();
};

class Scale : public Group {
    private:
        float x,y,z;
    public:
        Scale();
        Scale(float x, float y, float z);
        void setX(float n);
        void setY(float n);
        void setZ(float n);
        float getX();
        float getY();
        float getZ();
        void apply();
};

class Color{
    public:
        void virtual apply() = 0;
};

class Diffuse : public Color {
    float rgb[4];
    public:
        Diffuse(float r, float g, float b);
        void apply();
};

class Ambient : public Color {
    float rgb[4];
    public:
        Ambient(float r, float g, float b);
        void apply();
};

class Specular : public Color {
    float rgb[4];
    public:
        Specular(float r, float g, float b);
        void apply();
};

class Emissive : public Color {
    float rgb[4];
    public:
        Emissive(float r, float g, float b);
        void apply();
};

class Shininess : public Color {
    float s;
    public:
        Shininess(float t);
        void apply();
};

class Model {
    public:
        string model;
        GLuint texture;
        vector<Group*> transformations;
        vector<Color*> colors;
        GLuint vertices, verticeCount, normals, textures;
        Model();
        Model(string model, vector<Group*> t, vector<Color*>, GLuint texture);
};

class Light{
    public:
        int index;
        void virtual apply() = 0;
};

class LightPoint : public Light {
    float pos[4];
    public:
        LightPoint(float a, float b, float c, int i);
        void apply();
};

class LightDirectional : public Light{
    float dir[4];
    public:
        LightDirectional(float a, float b, float c, int i);
        void apply();
};

class LightSpotlight : public Light{
    float pos[4], dir[4], cutoff;
    public:
        LightSpotlight(float a, float b, float c, float da, float db, float dc, float ct, int i);
        void apply();
};

int getLight(int nLight);
