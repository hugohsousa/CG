#ifndef MAIN_CPP_CLASSES_H
#define MAIN_CPP_CLASSES_H
#endif //MAIN_CPP_CLASSES_H

#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <stdlib.h>
#include <GL/glut.h>
#endif

#include <string>
using namespace std;

class Group {
    private:
        int id;
    public:
        Group();
        Group(int id);
        void setId(int id);
        int getId();
        virtual void apply();
};

class Model : public Group {
    private:
        string file;
    public:
        Model();
        Model(string file);
        void setFile(string file);
        string getFile();
        void apply();
};

class Translate : public Group {
    private:
        float x,y,z;
    public:
        Translate();
        Translate(float x, float y, float z);
        void setX(float x);
        void setY(float y);
        void setZ(float z);
        float getX();
        float getY();
        float getZ();
        void apply();
};

class Rotate : public Group {
    private:
        float x,y,z,angle;
    public:
        Rotate();
        Rotate(float x, float y, float z, float angle);
        void setX(float x);
        void setY(float y);
        void setZ(float a);
        void setAngle(int angle);
        float getX();
        float getY();
        float getZ();
        float getAngle();
        void apply();
};

class Scale : public Group {
    private:
        float x,y,z;
    public:
        Scale();
        Scale(float x, float y, float z);
        void setX(float x);
        void setY(float y);
        void setZ(float z);
        float getX();
        float getY();
        float getZ();
        void apply();
};
