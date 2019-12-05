#include "carro.h"
#include "curva.h"

Carro::Carro(){
    modelo = new Model3DS("../3ds/taxicoo.3DS");

    u = 0.0;
    p = 0;
    direcao = 1;
}

void Carro::desenha(){
    glPushMatrix();
        Objeto::desenha();
        glRotatef(90, 1, 0, 0);
        GUI::drawOrigin(0);
        glScalef(0.025, 0.025, 0.025);
        modelo->draw();
    glPopMatrix();

}

void Carro::mover(){
    u += 0.0085 * (direcao);
    if(u >= 1){
        u = 0.0;
        p += direcao;
    }
    if(u < 0) {
        u = 0.99;
        p--;
    }
}

void Carro::mudaDirecao(){
    direcao *= (-1);
    mover();
}

void Carro::desenhaNaCurva(Curva &curva, const vector<Vetor3D> &pontosControle, Camera *camera){
    int n = pontosControle.size();
    if(p <= -n || p >= n)
        p = 0;
    vector<Vetor3D> pontos(4);

    for(int j = 0; j < 4; j++){
        pontos[j] = pontosControle[(p + j) % pontosControle.size()];
    }

    Vetor3D o= curva.getPoint(u, pontos);
    Vetor3D k= curva.getD1(u, pontos) * (-1);
    Vetor3D up = curva.getD2(u, pontos);
    Vetor3D i= up ^ k;
    Vetor3D j = k ^ i;

    k.normaliza();
    j.normaliza();
    i.normaliza();


    double T[] = {
        i.x, j.x, k.x, o.x,
        i.y, j.y, k.y, o.y,
        i.z, j.z, k.z, o.z,
        0   , 0   , 0   ,1
    };

    Vetor3D deslocamento = j*(-.6) + k*(1);
    camera->c = o + deslocamento;
    camera->u = up*-1;
    camera->e = o + k + deslocamento + j*(-.1);

    glPushMatrix();
        glMultTransposeMatrixd(T);
        this->desenha();
    glPopMatrix();


    glPushMatrix();
        glTranslatef(camera->e.x, camera->e.y, camera->e.z);
        glTranslatef(-o.x, -o.y, -o.z);
        glMultTransposeMatrixd(T);
        GUI::setColor(0.498, 0.498, 0.498);
    glPopMatrix();
}

