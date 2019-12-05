#include <iostream>

using namespace std;

#include <gui.h>
#include <vector>
#include "curva.h"
#include "carro.h"

Curva* curva = new Curva();
Carro* carro = new Carro();

Camera *c = new CameraDistante();
Camera *cAntiga = new CameraDistante();
bool moveCarro = false;
bool cameraP = true;
bool desenhaPontos = true;

//-------------------picking------------------
vector<Vetor3D> pontosControle;

int pontoSelecionado = 0;

void desenhaPontosDeControle(){
    //desenhando pontos de controle
    for (int i=0; i<pontosControle.size(); i++) {
        //definindo cor da selecao
        if (i == pontoSelecionado-1) {
            GUI::setColor(1,1,1,1,true);
        } else {
            GUI::setColor(0,0,1,1,true);
        }
        //desenhando (definindo nomes para o picking)
        Vetor3D p = pontosControle[i];
        glPushName(i+1); //não se deve definir name = 0!
            GUI::drawSphere(p.x,p.y,p.z,0.2);
        glPopName();
    }
}

//picking
int picking( GLint cursorX, GLint cursorY, int w, int h ) {
    int BUFSIZE = 512;
    GLuint selectBuf[512];

    GUI::pickingInit(cursorX,cursorY,w,h,selectBuf,BUFSIZE);

//de acordo com a implementacao original da funcao display
    //lembrar de nao inicializar a matriz de projecao, para nao ignorar a gluPickMatrix
    GUI::displayInit();
    //só precisa desenhar o que for selecionavel
    desenhaPontosDeControle();
//fim-de acordo com a implementacao original da funcao display

    //retornando o name do objeto (ponto de controle) mais proximo da camera (z minimo! *[matrizes de normalizacao da projecao])
    return GUI::pickingClosestName(selectBuf,BUFSIZE);
}
//-------------------picking------------------

//-------------------viewPorts------------------
bool viewports = false;
bool scissored = false;

void cenario();

//visao de duas cameras (duas viewports), viewport auxiliar sobrepondo a principal
void viewPorts() {
    float width = glutGUI::width;
    float height = glutGUI::height;

    //viewport principal
    glViewport(0, 0, width, height);
        glLoadIdentity();
        gluLookAt(glutGUI::cam->e.x,glutGUI::cam->e.y,glutGUI::cam->e.z, glutGUI::cam->c.x,glutGUI::cam->c.y,glutGUI::cam->c.z, glutGUI::cam->u.x,glutGUI::cam->u.y,glutGUI::cam->u.z);
            cenario();

    //viewport auxiliar sobrepondo a principal
    if (!scissored) {
        //misturando com a principal
        glViewport(0, 3*height/4, width/4, height/4);
    } else {
        //recortando/substituindo o pedaço
        GUI::glScissoredViewport(0, 3*height/4, width/4, height/4);
    }
        glLoadIdentity();

        if(!cameraP){
            gluLookAt(cAntiga->e.x,cAntiga->e.y,cAntiga->e.z, cAntiga->c.x,cAntiga->c.y,cAntiga->c.z, cAntiga->u.x,cAntiga->u.y,cAntiga->u.z);
        }

        gluLookAt(c->e.x,c->e.y,c->e.z, c->c.x,c->c.y,c->c.z, c->u.x,c->u.y,c->u.z);
        cenario();
}

//-------------------viewPorts------------------

void cenario() {
    //GUI::setLight(1,1,3,5,true,false);
    GUI::setLight(2,-1.5,0.5,-1,true,false);
    GUI::setLight(3,-5,3,-5,true,false);

    GUI::drawOrigin(0.5);

    //GUI::setColor(1,0,0);
    //GUI::drawFloor();
    GUI::setColor(0,0,0);
    Desenha::drawGrid(5, 0, 1, 1 );

    if(desenhaPontos){
        desenhaPontosDeControle();
    }
    curva->desenharCurva(pontosControle);
    carro->desenhaNaCurva(*curva,pontosControle,c);

    if(moveCarro){
        carro->mover();
    }
}

void desenha() {
    GUI::displayInit();

    if (!viewports) {
        cenario();
    } else {
        viewPorts();
    }

    //transladando ponto selecionado atraves do picking
    if (pontoSelecionado!=0) {
        pontosControle[pontoSelecionado-1].x += glutGUI::dtx;
        pontosControle[pontoSelecionado-1].y += glutGUI::dty;
        pontosControle[pontoSelecionado-1].z += glutGUI::dtz;
    }

    GUI::displayEnd();
}

void teclado(unsigned char key, int x, int y) {
    GUI::keyInit(key,x,y);

    switch (key) {
    case 't':
        glutGUI::trans_obj = !glutGUI::trans_obj;
        //transPontos = glutGUI::trans_obj;
        break;
    case 'l':
        glutGUI::trans_luz = !glutGUI::trans_luz;
        break;

    //Novo ponto de controle
    case 'p':
        pontosControle.push_back(Vetor3D(-1, 1, 0));
        break;
    //Excluir o ponto de controle selecionado
    case 'e':
        if(pontoSelecionado != 0)
            pontosControle.erase(pontosControle.begin()+pontoSelecionado-1, pontosControle.begin()+pontoSelecionado);
        break;

    //Carro anda automaticamente
    case 'a':
        moveCarro = !moveCarro;
        break;

    //Muda a direção que o carro vai andar(para frente ou para trás)
    case '.':
        carro->mudaDirecao();
        break;

    //Anda dependendo do usuário quando apertar a tecla
    case 'w':
        moveCarro = false;
        carro->mover();
        break;

    case 'g':
        if(cameraP){
            cAntiga = glutGUI::cam;
            glutGUI::cam = c;
        }else{
            glutGUI::cam = cAntiga;
        }
        cameraP = !cameraP;
        break;

    case 'P':
        desenhaPontos = !desenhaPontos;
        break;

    case '+':
        curva->aumentaDelta();
        break;

    case '-':
        curva->decrementaDelta();
        break;

    case 'I':
        curva->setMode(CurvaMode::INTERPOLADORA);
        break;

    case 'B':
        curva->setMode(CurvaMode::BEZIER);
        break;

    case 'H':
        curva->setMode(CurvaMode::HERMITE);
        break;

    case 'M':
        curva->setMode(CurvaMode::CATMULLROM);
        break;

    case 'S':
        curva->setMode(CurvaMode::BSPLINE);
        break;

    case 'v':
        viewports = !viewports;
        break;
    case 's':
        scissored = !scissored;
        break;

    default:
        break;
    }
}

void mouse(int button, int state, int x, int y) {
    GUI::mouseButtonInit(button,state,x,y);

    // if the left button is pressed
    if (button == GLUT_LEFT_BUTTON) {
        // when the button is pressed
        if (state == GLUT_DOWN) {
            //picking
            int pick = picking( x, y, 5, 5 );
            if (pick != 0) {
                pontoSelecionado = pick;
                glutGUI::lbpressed = false;
            }
        }
    }
}

int main(){

    pontosControle.push_back(Vetor3D(-4, 3, -1));
    pontosControle.push_back(Vetor3D(-2, 3, 1));
    pontosControle.push_back(Vetor3D(1, 3, 2));
    pontosControle.push_back(Vetor3D(4, 2, 1));
    pontosControle.push_back(Vetor3D(5, 2, -1));
    pontosControle.push_back(Vetor3D(4, 4, -1));

    GUI gui = GUI(800,600,desenha,teclado,mouse);
}
