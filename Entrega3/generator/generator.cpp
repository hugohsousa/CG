#define _USE_MATH_DEFINES
#include <cmath>
#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;
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

void drawSphere(float radius, int slices, int stacks, string fileName)
{
    ofstream fileSphere(fileName, ofstream::trunc);

    float stackStep = M_PI / stacks;
    float sliceStep = 2 * M_PI / slices;

    float a = 0.0f;
    float b = M_PI / 2;

    float p1x = 0.0f;
    float p1y = 0.0f;
    float p1z = 0.0f;

    float p2x = 0.0f;
    float p2y = 0.0f;
    float p2z = 0.0f;

    float p3x = 0.0f;
    float p3y = radius * sin(b);
    float p3z = 0.0f;

    float p4x = 0.0f;
    float p4y = 0.0f;
    float p4z = 0.0f;

    b -= stackStep;

    //Primeira stack
    for (int j = 0; j < slices; j++) {
        a = j * sliceStep;

        p1x = radius * cos(b) * sin(a);
        p1y = radius * sin(b);
        p1z = radius * cos(b) * cos(a);

        p2x = radius * cos(b) * sin(a + sliceStep);
        p2y = radius * sin(b);
        p2z = radius * cos(b) * cos(a + sliceStep);

        fileSphere << p1x << " " << p1y << " " << p1z << endl;
        fileSphere << p2x << " " << p2y << " " << p2z << endl;
        fileSphere << p3x << " " << p3y << " " << p3z << endl;

    }
    //Stacks
    for (int i = 1; i < stacks - 1; i++) {

        a = 0.0f;
        b = M_PI / 2 - i * stackStep;

        for (int j = 0; j < slices; j++) {
            a = j * sliceStep;

            p3x = radius * cos(b) * sin(a);
            p3y = radius * sin(b);
            p3z = radius * cos(b) * cos(a);

            p4x = radius * cos(b) * sin(a + sliceStep);
            p4y = radius * sin(b);
            p4z = radius * cos(b) * cos(a + sliceStep);

            p2x = radius * cos(b - stackStep) * sin(a + sliceStep);
            p2y = radius * sin(b - stackStep);
            p2z = radius * cos(b - stackStep) * cos(a + sliceStep);

            p1x = radius * cos(b - stackStep) * sin(a);
            p1y = radius * sin(b - stackStep);
            p1z = radius * cos(b - stackStep) * cos(a);

            fileSphere << p1x << " " << p1y << " " << p1z << endl;
            fileSphere << p2x << " " << p2y << " " << p2z << endl;
            fileSphere << p4x << " " << p4y << " " << p4z << endl;

            fileSphere << p1x << " " << p1y << " " << p1z << endl;
            fileSphere << p4x << " " << p4y << " " << p4z << endl;
            fileSphere << p3x << " " << p3y << " " << p3z << endl;

        }
    }

    b = M_PI / 2 - (stacks - 1)*stackStep;

    p3x = 0.0f;
    p3y = radius * sin(b - stackStep);
    p3z = 0.0f;
    //Última stack
	for (int j = 0; j < slices; j++) {
        a = j * sliceStep;

        p1x = radius * cos(b) * sin(a);
        p1y = radius * sin(b);
        p1z = radius * cos(b) * cos(a);

        p2x = radius * cos(b) * sin(a + sliceStep);
        p2y = radius * sin(b);
        p2z = radius * cos(b) * cos(a + sliceStep);

        fileSphere << p1x << " " << p1y << " " << p1z << endl;
        fileSphere << p3x << " " << p3y << " " << p3z << endl;
        fileSphere << p2x << " " << p2y << " " << p2z << endl;

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
