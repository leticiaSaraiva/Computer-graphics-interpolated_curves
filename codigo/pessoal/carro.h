#ifndef CARRO_H
#define CARRO_H

#include <curva.h>
#include "model3ds.h"
#include "objeto.h"

class Carro : public Objeto
{
public:
    Model3DS *modelo;

    float u;
    int p;
    int direcao;

    Carro();
    void desenha();
    void mover();
    void mudaDirecao();
    void desenhaNaCurva(Curva &curva, const vector<Vetor3D> &pontosControle, Camera *camera);
};

#endif // CARRO_H
