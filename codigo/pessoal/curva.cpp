#include <math.h>
#include "curva.h"
#include "Desenha.h"
#include "gui.h"

Curva::Curva(){
    setMode(CurvaMode::BSPLINE);
    delta = .01;
}

void Curva::aumentaDelta(){
    if(delta < 1)
        delta += .001;
}
void Curva::decrementaDelta(){
    if(delta > 0.01)
        delta -= .001;
}

void Curva::setMode(CurvaMode modex){
    mode = modex;

    if(mode == CurvaMode::INTERPOLADORA){
        float Maux[4][4] = {
            {-4.5, 13.5, -13.5, 4.5},
            {9, -22.5, 18, -4.5},
            {-5.5, 9, -4.5, 1},
            {1, 0, 0, 0}
        };

        for(int i = 0; i<4; i++){
            for(int j = 0; j<4; j++){
                M[i][j] = Maux[i][j];
            }
        }
    }
    else if(mode == CurvaMode::HERMITE){
        float Maux[4][4] = {
            {2, -2, 1, 1},
            {-3, 3, -2, -1},
            {0, 0, 1, 0},
            {1, 0, 0, 0}
        };

        for(int i = 0; i<4; i++){
            for(int j = 0; j<4; j++){
                M[i][j] = Maux[i][j];
            }
        }
    }
    else if(mode == CurvaMode::BEZIER){
        float Maux[4][4] = {
            {-1, 3, -3, 1},
            {3, -6, 3, 0},
            {-3, 3, 0, 0},
            {1, 0, 0, 0}
        };

        for(int i = 0; i<4; i++){
            for(int j = 0; j<4; j++){
                M[i][j] = Maux[i][j];
            }
        }

    }
    else if(mode == CurvaMode::BSPLINE){
        float Maux[4][4] = {
            {-1/6.0, 3/6.0, -3/6.0, 1/6.0},
            {3/6.0, -6/6.0, 3/6.0, 0/6.0},
            {-3/6.0, 0/6.0, 3/6.0, 0/6.0},
            {1/6.0, 4/6.0, 1/6.0, 0/6.0}
        };

        for(int i = 0; i<4; i++){
            for(int j = 0; j<4; j++){
                M[i][j] = Maux[i][j];
            }
        }
    }
    else if(mode == CurvaMode::CATMULLROM){

        float Maux[4][4] = {
            {-1/2.0, 3/2.0, -3/2.0, 1/2.0},
            {2/2.0, -5/2.0, 4/2.0, -1/2.0},
            {-1/2.0, 0.0, 1/2.0, 0.0},
            {0.0, 2/2.0, 0.0, 0.0}
        };

        for(int i = 0; i<4; i++){
            for(int j = 0; j<4; j++){
                M[i][j] = Maux[i][j];
            }
        }
    }
}

CurvaMode Curva::getMode(){
    return mode;
}

Vetor3D Curva::getPoint(float u, const vector<Vetor3D> &points){

    Vetor3D resultado;
    double us[4] = {pow(u, 3), pow(u, 2), u, 1};

    double matriz[4];

    for(int i = 0; i<4; i++){
        matriz[i] = 0;
        for(int j = 0; j<4; j++){
            matriz[i] += us[j] * M[j][i];
        }
    }

    for(int i = 0; i<4; i++){
        resultado.x += matriz[i] * points[i].x;
        resultado.y += matriz[i] * points[i].y;
        resultado.z += matriz[i] * points[i].z;
    }

    return resultado;
}

Vetor3D Curva::getD1(float u, const vector<Vetor3D> &points){

    Vetor3D resultado;
    float us[] = {3*pow(u, 2), 2*u, 1, 0};

    float matriz[4];

    for(int i = 0; i<4; i++){
        matriz[i] = 0;
        for(int j = 0; j<4; j++){
            matriz[i] += us[j] * M[j][i];
        }
    }

    for(int i = 0; i<4; i++){
        resultado.x += matriz[i] * points[i].x;
        resultado.y += matriz[i] * points[i].y;
        resultado.z += matriz[i] * points[i].z;
    }

    return resultado;

}

Vetor3D Curva::getD2(float u, const vector<Vetor3D> &points){

    Vetor3D resultado;
    float us[] = {6*u, 2, 0, 0};

    float matriz[4];

    for(int i = 0; i<4; i++){
        matriz[i] = 0;
        for(int j = 0; j<4; j++){
            matriz[i] += us[j] * M[j][i];
        }
    }

    for(int i = 0; i<4; i++){
        resultado.x += matriz[i] * points[i].x;
        resultado.y += matriz[i] * points[i].y;
        resultado.z += matriz[i] * points[i].z;
    }

    return resultado;

}


void Curva::desenharCurva(const vector<Vetor3D> &pontosControle){

    vector<Vetor3D> pontos(4);

    CurvaMode t = getMode();
    int a = 1;

    if(t == CurvaMode::BSPLINE || t == CurvaMode::CATMULLROM){
        a = pontosControle.size();
    }
    for(int i = 0; i < a; i++){
        for(int j = 0; j < 4; j++){
            pontos[j] = pontosControle[(i + j) % pontosControle.size()];
        }

        for(float u = 0; u <= 1; u += delta){
            Vetor3D o_= getPoint(u, pontos);
            Vetor3D k_= getD1(u, pontos) * (-1);
            Vetor3D up = getD2(u, pontos);
            Vetor3D i_= up ^ k_;
            Vetor3D j_ = k_ ^ i_;

            k_.normaliza();
            j_.normaliza();
            i_.normaliza();

            double T[] = {
                i_.x, j_.x, k_.x, o_.x,
                i_.y, j_.y, k_.y, o_.y,
                i_.z, j_.z, k_.z, o_.z,
                0   , 0   , 0   , 1
            };

            glPushMatrix();
                glMultTransposeMatrixd(T);
                desenharTrechoLocal();
            glPopMatrix();
        }
    }

}

void Curva::desenharTrechoLocal(){

    glDisable(GL_CULL_FACE);
    glPushMatrix();
        glTranslated(0.25, 0, 0);
        glRotated(90, 0, 1, 0);
        glRotated(180, 1, 0, 0);
        glPushMatrix();
            GUI::setColor(.2,1,.2);
            Desenha::drawQuadFace(Vetor3D(0,0,0), Vetor3D(.3, .1, -.2));
            GUI::setColor(0,0,0);
            Desenha::drawQuadFace(Vetor3D(0,0,0), Vetor3D(.3, 0, .6));
            GUI::setColor(.2,1,.2);
            Desenha::drawQuadFace(Vetor3D(0, 0, .6), Vetor3D(.3, .1, .8));
            GUI::setColor(1,1,1);
            Desenha::drawQuadFace(Vetor3D(0, 0.001, .29), Vetor3D(.3, 0.001, .31));
        glPopMatrix();
    glPopMatrix();
}
