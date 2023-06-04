#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstring>
#include <vector>

using namespace std;
float* formulaBezier(float t, float* p0, float* p1, float* p2, float *p3){
    float* result = new float[3];

    float t1 = (1-t);

    //(1-t)^3
    float b0 = t1 * t1 * t1; 
    //3*t*(1-t)^2
    float b1 = 3 * (t1 * t1) * t;
    //3*(t^2)*(1-t) 
    float b2 = 3 * t1 * (t * t); 
    // t^3
    float b3 = t * t * t; 


    result[0] = b0 * p0[0] + b1 * p1[0] + b2 * p2[0] + b3 * p3[0];
    result[1] = b0 * p0[1] + b1 * p1[1] + b2 * p2[1] + b3 * p3[1];
    result[2] = b0 * p0[2] + b1 * p1[2] + b2 * p2[2] + b3 * p3[2];

    return result;


}

float* bezier(float n, float m, float** points, int* index){
    int i;
    float* point = new float[3];
    float* result = new float[3];
    int j = 0;
    int N = 0;
    //pontos de controlo
    float controlPoints[4][3];
    //pontos da curva de bezier 
    float bezierPoints[4][3];

    for(i=0; i < 16; i++){

        controlPoints[j][0] = points[index[i]][0];
        controlPoints[j][1] = points[index[i]][1];
        controlPoints[j][2] = points[index[i]][2];
        j++;

        //4 em 4 pontos
        if(j % 4 == 0){

            //ponto da curva de bezier
            point = formulaBezier(n, controlPoints[0], controlPoints[1], controlPoints[2], controlPoints[3]);

            bezierPoints[N][0] = point[0];
            bezierPoints[N][1] = point[1];
            bezierPoints[N][2] = point[2];
            j = 0;
            N++;
        }
    }
    
    result = formulaBezier(m, bezierPoints[0], bezierPoints[1], bezierPoints[2], bezierPoints[3]);

    return result;
}

void drawBezier(string patchFile, int tessellation){

    //Abre o .patch para ler
    ifstream read(patchFile); 
    //Abre o bezier para escrever
    ofstream write("../../3d/beziercurve.3d"); 
    string line, value;
    int IndLines, pointsLines, nPatches, p1, readCoords;
    int position;

    float subdiv = 1.0 / tessellation;

    
    if(read.is_open()){ 

        //primeira linha
        getline(read, line); 
        //numero de patches
        int numPatches = atoi(line.c_str()); 
        int** indControlPoints = new int*[numPatches]; 
        float*** finalPoints = new float**[numPatches]; 

        //Indices de cada patch
        for(IndLines = 0; IndLines < numPatches; IndLines++){
            getline(read, line);
            //Array com 16 pontos de controlo
            indControlPoints[IndLines] = new int[16];

            for(p1 = 0; p1 < 16; p1++){
                position = line.find(',');
                value = line.substr(0, position);
                indControlPoints[IndLines][p1] = atoi(value.c_str());
                line.erase(0, position + 1);
            }
        }

        //Ler numero de pontos 
        getline(read, line); 
        int numPoints = atoi(line.c_str());
        float** points = new float*[numPoints];



        for(pointsLines = 0; pointsLines < numPoints; pointsLines++){
            getline(read, line);
            //array de 3 floats para as coordenadas
            points[pointsLines] = new float[3]; 

            for(readCoords = 0; readCoords < 3; readCoords++){
                position = line.find(",");
                value = line.substr(0, position);
                points[pointsLines][readCoords] = atof(value.c_str());

                line.erase(0, position + 1);
            }
        }

        for(nPatches = 0; nPatches < numPatches; nPatches++){
            finalPoints[nPatches] = new float*[4];//[32][4]

            for(int lin = 0; lin < tessellation; lin++ ) {

                for(int col = 0; col < tessellation; col++) {

                    float x1 = subdiv * lin;
                    float y1 = subdiv * col;
                    float x2 = subdiv * (lin + 1 );
                    float y2 = subdiv * (col + 1 );

                    finalPoints[nPatches][0] = bezier(x1, y1, points, indControlPoints[nPatches]);
                    finalPoints[nPatches][1] = bezier(x1, y2, points, indControlPoints[nPatches]);
                    finalPoints[nPatches][2] = bezier(x2, y1, points, indControlPoints[nPatches]);
                    finalPoints[nPatches][3] = bezier(x2, y2, points, indControlPoints[nPatches]);


                    write << finalPoints[nPatches][0][0] << " " << finalPoints[nPatches][0][1] << " " << finalPoints[nPatches][0][2] << endl;
                    write << finalPoints[nPatches][2][0] << " " << finalPoints[nPatches][2][1] << " " << finalPoints[nPatches][2][2] << endl;
                    write << finalPoints[nPatches][3][0] << " " << finalPoints[nPatches][3][1] << " " << finalPoints[nPatches][3][2] << endl;

                    write << finalPoints[nPatches][0][0] << " " << finalPoints[nPatches][0][1] << " " << finalPoints[nPatches][0][2] << endl;
                    write << finalPoints[nPatches][3][0] << " " << finalPoints[nPatches][3][1] << " " << finalPoints[nPatches][3][2] << endl;
                    write << finalPoints[nPatches][1][0] << " " << finalPoints[nPatches][1][1] << " " << finalPoints[nPatches][1][2] << endl;

                }
            }
        }

        write.close();
        read.close();
    } else {
        printf("Ficheiro.patch Inválido!");
    }
}

void drawPlane(float unit, int divisions, string fileName)
{

    ofstream filePlane(fileName, ios::trunc);

    float ud = -((unit * divisions) / 2);
    float udu = -((unit * divisions) / 2 - unit);

    for (int i = 0; i < divisions; i++)
    {
        for (int j = 0; j < divisions; j++)
        {
            //Triangulos 1
            filePlane << udu + j * unit << " 0 " << ud + i * unit << endl;
            filePlane << ud + j * unit << " 0 " << ud + i * unit << endl;
            filePlane << udu + j * unit << " 0 " << udu + i * unit << endl;
            //Triangulos 2
            filePlane << ud + j * unit << " 0 " << ud + i * unit << endl;
            filePlane << ud + j * unit << " 0 " << udu + i * unit << endl;
            filePlane << udu + j * unit << " 0 " << udu + i * unit << endl;
        }
    }
    filePlane.close();
}

void drawBox(float unit, int divisions, string fileName)
{

    ofstream fileBox(fileName, ofstream::trunc);

    float ud = -((unit * divisions) / 2);
    float udu = -((unit * divisions) / 2 - unit);
    float y = (unit * divisions) / 2;

    for (int i = 0; i < divisions; i++)
    {
        for (int j = 0; j < divisions; j++)
        {

            // Top
            fileBox << udu + j * unit << ' ' << y << ' ' << ud + i * unit << endl;
            fileBox << ud + j * unit << ' ' << y << ' ' << ud + i * unit << endl;
            fileBox << udu + j * unit << ' ' << y << ' ' << udu + i * unit << endl;

            fileBox << ud + j * unit << ' ' << y << ' ' << ud + i * unit << endl;
            fileBox << ud + j * unit << ' ' << y << ' ' << udu + i * unit << endl;
            fileBox << udu + j * unit << ' ' << y << ' ' << udu + i * unit << endl;

            // Bottom
            fileBox << ud + j * unit << ' ' << -y << ' ' << ud + i * unit << endl;
            fileBox << udu + j * unit << ' ' << -y << ' ' << ud + i * unit << endl;
            fileBox << udu + j * unit << ' ' << -y << ' ' << udu + i * unit << endl;

            fileBox << ud + j * unit << ' ' << -y << ' ' << udu + i * unit << endl;
            fileBox << ud + j * unit << ' ' << -y << ' ' << ud + i * unit << endl;
            fileBox << udu + j * unit << ' ' << -y << ' ' << udu + i * unit << endl;

            // Rigth
            fileBox << y << ' ' << udu + j * unit << ' ' << ud + i * unit << endl;
            fileBox << y << ' ' << udu + j * unit << ' ' << udu + i * unit << endl;
            fileBox << y << ' ' << ud + j * unit << ' ' << ud + i * unit << endl;

            fileBox << y << ' ' << ud + j * unit << ' ' << ud + i * unit << endl;
            fileBox << y << ' ' << udu + j * unit << ' ' << udu + i * unit << endl;
            fileBox << y << ' ' << ud + j * unit << ' ' << udu + i * unit << endl;

            // Left
            fileBox << -y << ' ' << udu + j * unit << ' ' << ud + i * unit << endl;
            fileBox << -y << ' ' << ud + j * unit << ' ' << ud + i * unit << endl;
            fileBox << -y << ' ' << udu + j * unit << ' ' << udu + i * unit << endl;

            fileBox << -y << ' ' << udu + j * unit << ' ' << udu + i * unit << endl;
            fileBox << -y << ' ' << ud + j * unit << ' ' << ud + i * unit << endl;
            fileBox << -y << ' ' << ud + j * unit << ' ' << udu + i * unit << endl;

            // Front
            fileBox << ud + j * unit << ' ' << ud + i * unit << ' ' << y << endl;
            fileBox << udu + j * unit << ' ' << ud + i * unit << ' ' << y << endl;
            fileBox << udu + j * unit << ' ' << udu + i * unit << ' ' << y << endl;

            fileBox << ud + j * unit << ' ' << udu + i * unit << ' ' << y << endl;
            fileBox << ud + j * unit << ' ' << ud + i * unit << ' ' << y << endl;
            fileBox << udu + j * unit << ' ' << udu + i * unit << ' ' << y << endl;

            // Back
            fileBox << udu + j * unit << ' ' << ud + i * unit << ' ' << -y << endl;
            fileBox << ud + j * unit << ' ' << ud + i * unit << ' ' << -y << endl;
            fileBox << udu + j * unit << ' ' << udu + i * unit << ' ' << -y << endl;

            fileBox << ud + j * unit << ' ' << ud + i * unit << ' ' << -y << endl;
            fileBox << ud + j * unit << ' ' << udu + i * unit << ' ' << -y << endl;
            fileBox << udu + j * unit << ' ' << udu + i * unit << ' ' << -y << endl;
        }
    }
    fileBox.close();
}

void drawCone(float radius, float height, int slices, int stacks, string fileName)
{

    ofstream fileCone(fileName, ofstream::trunc);

    float a = 0.0f;
    float step = 2 * M_PI / slices;

    for (int i = 0; i <= slices; i++)
    {
        // Base
        fileCone << sin(a) * radius << " 0 " << cos(a) * radius << endl;
        fileCone << "0" << " 0 " << "0" << endl;
        a += step;
        fileCone << sin(a) * radius << " 0 " << cos(a) * radius << endl;

        // Stacks - 1 
        for (int j = 0; j < stacks - 1; j++)
        {
            int sj = stacks - j;
            a -= step;
            fileCone << (sj - 1) * (sin(a) * radius) / stacks << " " << (j + 1) * height / stacks << " " << (sj - 1) * (cos(a) * radius) / stacks << endl;
            fileCone << (sj) * ((sin(a) * radius) / stacks) << " " << (j)*height / stacks << " " << (sj) * ((cos(a) * radius) / stacks) << endl;
            a += step;
            fileCone << (sj) * ((sin(a) * radius) / stacks) << " " << (j)*height / stacks << " " << (sj) * ((cos(a) * radius) / stacks) << endl;

            a -= step;
            fileCone << (sj - 1) * (sin(a) * radius) / stacks << " " << (j + 1) * height / stacks << " " << (sj - 1) * (cos(a) * radius) / stacks << endl;
            a += step;
            fileCone << (sj - 1) * (sin(a) * radius) / stacks << " " << (j + 1) * height / stacks << " " << (sj - 1) * (cos(a) * radius) / stacks << endl;
            fileCone << (sj) * ((sin(a) * radius) / stacks) << " " << (j)*height / stacks << " " << (sj) * ((cos(a) * radius) / stacks) << endl;
        }

        // Última Stack
        int j = stacks - 1;
        int sj = stacks - j;
        a -= step;
        fileCone << (sj - 1) * (sin(a) * radius) / stacks << " " << (j + 1) * height / stacks << " " << (sj - 1) * (cos(a) * radius) / stacks << endl;
        fileCone << (sj) * ((sin(a) * radius) / stacks) << " " << (j)*height / stacks << " " << (sj) * ((cos(a) * radius) / stacks) << endl;
        a += step;
        fileCone << (sj) * ((sin(a) * radius) / stacks) << " " << (j)*height / stacks << " " << (sj) * ((cos(a) * radius) / stacks) << endl;
    }
    fileCone.close();
}

void drawSphere(float radius, int slices, int stacks, string fileName) {

    ofstream fileSphere(fileName, ios::trunc );

    float stackStep = (float) M_PI / stacks;
    float sliceStep = 2 * (float) M_PI / slices;

    float alpha = 0.0f;         
    float betha = (float) M_PI / 2;        

    float p1x;
    float p1y;
    float p1z;

    float p2x;
    float p2y;
    float p2z;

    float p3x;
    float p3y;
    float p3z;

    float p4x;
    float p4y;
    float p4z;

    vector<float> textura, normais;

    float textureX;
    float textureY;

    for (int i = 0; i < stacks ; i++) {

        alpha = 0.0f;
        betha = (float) M_PI / 2 - i * stackStep;

        textureY = ((float)i/(float)stacks);

        for (int j = 0; j < slices; j++) {

            textureX = (float) j / (float) slices;

            alpha = j * sliceStep;

            p3x = radius * cos(betha) * sin(alpha);
            p3y = radius * sin(betha);
            p3z = radius * cos(betha) * cos(alpha);

            p4x = radius * cos(betha) * sin(alpha + sliceStep);
            p4y = radius * sin(betha);
            p4z = radius * cos(betha) * cos(alpha + sliceStep);

            p2x = radius * cos(betha - stackStep) * sin(alpha + sliceStep);
            p2y = radius * sin(betha - stackStep);
            p2z = radius * cos(betha - stackStep) * cos(alpha + sliceStep);

            p1x = radius * cos(betha - stackStep) * sin(alpha);
            p1y = radius * sin(betha - stackStep);
            p1z = radius * cos(betha - stackStep) * cos(alpha);

            // vertice
            // normal
            // textura

            fileSphere << p1x << " " << p1y << " " << p1z << " "  << p1x/radius << " " << p1y/radius << " " << p1z/radius << " "<<(asin( p1x/radius) /(float)M_PI) + 0.5  << " " << (asin(p1y/radius) /(float)M_PI) + 0.5 << " "  << endl;

            fileSphere << p2x << " " << p2y << " " << p2z  << " "<< p2x/radius << " " << p2y/radius << " " << p2z/radius << " "  <<  (asin( p2x/radius) /(float)M_PI) + 0.5 << " " << (asin(p2y/radius) /(float)M_PI) + 0.5 << " "  << endl;

            fileSphere << p4x << " " << p4y << " " << p4z << " " << p4x/radius << " " << p4y/radius << " " << p4z/radius << " "  << (asin( p4x/radius) /(float)M_PI) + 0.5 << " " << (asin(p4y/radius) /(float)M_PI) + 0.5 << " "  << endl;



            fileSphere << p1x << " " << p1y << " " << p1z << " "  << p1x/radius << " " << p1y/radius << " " << p1z/radius << " "<< (asin(p1x/radius) /(float) M_PI) + 0.5  << " " << (asin(p1y/radius) /(float)M_PI) + 0.5 << " "  << endl;

            fileSphere << p4x << " " << p4y << " " << p4z << " " << p4x/radius << " " << p4y/radius << " " << p4z/radius << " "  << (asin(p4x/radius) /(float) M_PI) + 0.5 << " " << (asin(p4y/radius) /(float)M_PI) + 0.5<< " "  << endl;

            fileSphere << p3x << " " << p3y << " " << p3z << " " << p3x/radius << " " << p3y/radius << " " << p3z/radius << " " << (asin(p3x/radius) / (float) M_PI) + 0.5 << " " << (asin(p3y/radius) /(float)M_PI) + 0.5<< " " << endl;


        }
    }

    fileSphere.close();
}


int main(int argc, char *argv[])
{
    if (std::strcmp(argv[1], "plane") == 0)
    {
        if (argc != 5)
        {
            cout << "\nIntroduza plano dimensao divisões ficheiro de destino\n";
            exit(1);
        }
        // Váriáveis necessárias para criar a primitiva
        float unit = atof(argv[2]);
        float divisions = atoi(argv[3]);
        string fileName = string(argv[4]);
        drawPlane(unit, divisions, fileName);
        return 1;
    }
    
    if (std::strcmp(argv[1], "box") == 0)
    {
        if (argc != 5)
        {
            cout << "\nIntroduza box dimensão divisoes ficheiro de destino\n";
            exit(1);
        }
        // Váriáveis necessárias para criar a primitiva
        float unit = atof(argv[2]);
        float divisions = atoi(argv[3]);
        string fileName = string(argv[4]);
        drawBox(unit, divisions, fileName);
        return 1;
    }
    if (std::strcmp(argv[1], "sphere") == 0)
    {
        if (argc != 6)
        {
            cout << "\nIntroduza sphere raio slices stacks ficheiro de destino\n";
            exit(1);
        }
        // Váriáveis necessárias para criar as primitivas
        float radius = atof(argv[2]);
        int slices = atoi(argv[3]);
        int stacks = atoi(argv[4]);
        string fileName = string(argv[5]);
        drawSphere(radius, slices, stacks, fileName);
        return 1;
    }

    if (std::strcmp(argv[1], "cone") == 0)
    {
        if (argc != 7)
        {
            cout << "\nIntroduza cone raio altura slices stacks ficheiro de destino\n";
            exit(1);
        }
        // Váriáveis necessárias para criar as primitivas
        float radius = atof(argv[2]);
        float height = atof(argv[3]);
        int slices = atoi(argv[4]);
        int stacks = atoi(argv[5]);
        string fileName = string(argv[6]);
        drawCone(radius, height, slices, stacks, fileName);
        return 1;
    }
    if (std::strcmp(argv[1], "bezier") == 0) {
        
        if (argc != 4) {
            cout << "\nIntroduza bazier ficheiro.patch tessellation\n";
            exit(1);
        }
        string file = argv[2];
        int tessellation = (atoi(argv[3]));
        drawBezier(file, tessellation);
        return 1;
        
    }
    else
    {
        cout << "\nFigura nao reconhecida\nFiguras disponiveis: plane,box,sphere,cone\n";
        return -1;  
    }
}
