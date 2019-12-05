#ifndef CURVA_H
#define CURVA_H

#include "Vetor3D.h"
#include <vector>
#include <iostream>
#include <cmath>
#include <GL/gl.h>
#include <GL/glut.h>
//#include "pessoal/carro.h"

using namespace std;

enum CurvaMode{
    INTERPOLADORA, BEZIER, HERMITE, BSPLINE, CATMULLROM
};

class Curva{


public:

    Vetor3D p[4];
    float M[4][4];
    float delta;
    CurvaMode mode;

    Curva();

    void setMode(CurvaMode);

    CurvaMode getMode();

    void decrementaDelta();

    void aumentaDelta();

    Vetor3D getPoint(float u, const vector<Vetor3D> &points);

    Vetor3D getD1(float u, const vector<Vetor3D> &points);

    Vetor3D getD2(float u, const vector<Vetor3D> &points);

    void desenharCurva(const vector<Vetor3D> &pontosControle);

    void desenharTrechoLocal();
};

#endif // CURVA_H
