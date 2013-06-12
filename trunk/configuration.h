#ifndef CONFIGURATION_H
#define CONFIGURATION_H

#include <network.h>
#include <QGroupBox>
#include <QComboBox>
#include <QMouseEvent>
#include <QObject>
#include <QLabel>

#define len(x) sizeof(x)/sizeof(byte)
#define non(x) (byte)-(x)

/** ========================================================================
 * @brief Motor class  class controls the motors lego
 */
class Motor{
public:
    byte vel;
    byte dir;
    bool state;
    byte motor;

    /** ----------------------------------------------------------------------
    * @brief constructor set the initial values
    */
    Motor(byte m):motor(m){
        vel=0;
        dir=0;
        state=false;
    }

    /** ----------------------------------------------------------------------
    * @brief run, start de motor
    */
    void run(Network *n){
        byte c[]={0x04, motor,dir?vel:non(vel),
                         0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00};
        n->directCommand(c,len(c));
    }

};

/** ========================================================================
 * @brief configuration class integrates the three engines and the respective key
 * inherited from QGroupBox to attach the Qlabel and QComboBox
 */
class Configuration:public QGroupBox{
    Q_OBJECT
signals:

    void configurationClicked(Configuration *s);

public:

    Motor           *motorA;
    Motor           *motorB;
    Motor           *motorC;
    QComboBox       *key;
    Configuration   *next;
    Configuration   *back;
    QLabel          *label;
    int             k;

    /** ----------------------------------------------------------------------
    * @brief constructor, set initial values
    */
    Configuration(QString n,QGroupBox *w):QGroupBox(n,w){

        next    = NULL;
        back    = NULL;
        motorA  = new Motor(0x00);
        motorB  = new Motor(0x01);
        motorC  = new Motor(0x02);
        key     = new QComboBox(this);
        label   = new QLabel("Key",this);
        k=0;
        label->setGeometry(100,15,70,30);
        key->setGeometry(130,15,70,30);
        setStyleSheet("QGroupBox {border: 1px solid gray;border-radius: 9px;margin-top: 0.5em;}");
        setKeys();
    }

    /** ----------------------------------------------------------------------
    * @brief change color, change the border color, between red or black
    */
    void chageColor(int v){
        if(v==1) setStyleSheet("QGroupBox {border: 1px solid red;border-radius: 9px;margin-top: 0.5em;}");
        else setStyleSheet("QGroupBox {border: 1px solid black;border-radius: 9px;margin-top: 0.5em;}");
    }

    /** ----------------------------------------------------------------------
    * @brief getKey, returns the key selected in QComboBox
    */
    int getKey(){
        switch (key->currentIndex()){
                case 0:
                    return Qt::Key_Up;
                    break;
                case 1:
                    return Qt::Key_Down;
                    break;
                case 2:
                    return Qt::Key_Right;
                    break;
                case 3:
                    return Qt::Key_Left;
                    break;
                case 4:
                    return Qt::Key_A;
                    break;
                case 5:
                    return Qt::Key_B;
                    break;
                case 6:
                    return Qt::Key_C;
                    break;
                case 7:
                    return Qt::Key_D;
                    break;
                case 8:
                    return Qt::Key_E;
                    break;
                case 9:
                    return Qt::Key_F;
                    break;
                case 10:
                    return Qt::Key_G;
                    break;
                case 11:
                    return Qt::Key_H;
                    break;
                case 12:
                    return Qt::Key_I;
                    break;
                case 13:
                    return Qt::Key_J;
                    break;
                case 14:
                    return Qt::Key_K;
                    break;
                case 15:
                    return Qt::Key_L;
                    break;
                case 16:
                    return Qt::Key_M;
                    break;
                case 17:
                    return Qt::Key_N;
                    break;
                case 18:
                    return Qt::Key_O;
                    break;
                case 19:
                    return Qt::Key_P;
                    break;
                case 20:
                    return Qt::Key_Q;
                    break;
                case 21:
                    return Qt::Key_R;
                    break;
                case 22:
                    return Qt::Key_S;
                    break;
                case 23:
                    return Qt::Key_T;
                    break;
                case 24:
                    return Qt::Key_U;
                    break;
                case 25:
                    return Qt::Key_V;
                    break;
                case 26:
                    return Qt::Key_W;
                    break;
                case 27:
                    return Qt::Key_X;
                    break;
                case 28:
                    return Qt::Key_Y;
                    break;
                case 29:
                    return Qt::Key_Z;
                    break;
        }

        return 0;

    }

    /** ----------------------------------------------------------------------
    * @brief changeKey, updates the key on the QComboBox when loading from file
    */
    void changeKey(int c){

        switch (c){
                case Qt::Key_Up:
                    key->setCurrentIndex(0);
                    break;
                case Qt::Key_Down:
                    key->setCurrentIndex(1);
                    break;
                case Qt::Key_Right:
                    key->setCurrentIndex(2);
                    break;
                case Qt::Key_Left:
                    key->setCurrentIndex(3);
                    break;
                case Qt::Key_A:
                    key->setCurrentIndex(4);
                    break;
                case Qt::Key_B:
                    key->setCurrentIndex(5);
                    break;
                case Qt::Key_C:
                    key->setCurrentIndex(6);
                    break;
                case Qt::Key_D:
                    key->setCurrentIndex(7);
                    break;
                case Qt::Key_E:
                    key->setCurrentIndex(8);
                    break;
                case Qt::Key_F:
                    key->setCurrentIndex(9);
                    break;
                case Qt::Key_G:
                    key->setCurrentIndex(10);
                    break;
                case Qt::Key_H:
                    key->setCurrentIndex(11);
                    break;
                case Qt::Key_I:
                    key->setCurrentIndex(12);
                    break;
                case Qt::Key_J:
                    key->setCurrentIndex(13);
                    break;
                case Qt::Key_K:
                    key->setCurrentIndex(14);
                    break;
                case Qt::Key_L:
                    key->setCurrentIndex(15);
                    break;
                case Qt::Key_M:
                    key->setCurrentIndex(16);
                    break;
                case Qt::Key_N:
                    key->setCurrentIndex(17);
                    break;
                case Qt::Key_O:
                    key->setCurrentIndex(18);
                    break;
                case Qt::Key_P:
                    key->setCurrentIndex(19);
                    break;
                case Qt::Key_Q:
                    key->setCurrentIndex(20);
                    break;
                case Qt::Key_R:
                    key->setCurrentIndex(21);
                    break;
                case Qt::Key_S:
                    key->setCurrentIndex(22);
                    break;
                case Qt::Key_T:
                    key->setCurrentIndex(23);
                    break;
                case Qt::Key_U:
                    key->setCurrentIndex(24);
                    break;
                case Qt::Key_V:
                    key->setCurrentIndex(25);
                    break;
                case Qt::Key_W:
                    key->setCurrentIndex(26);
                    break;
                case Qt::Key_X:
                    key->setCurrentIndex(27);
                    break;
                case Qt::Key_Y:
                    key->setCurrentIndex(28);
                    break;
                case Qt::Key_Z:
                    key->setCurrentIndex(29);
                    break;
        }
    }

    /** ----------------------------------------------------------------------
    * @brief run, engine starts according to your settings
    */
    void run(Network *n){
        if(motorA->state)motorA->run(n);
        if(motorB->state)motorB->run(n);
        if(motorC->state)motorC->run(n);
    }

protected:

    /** ----------------------------------------------------------------------
    * @brief mousePressEvent, if clicked, returns it settings
    */
    void mousePressEvent(QMouseEvent *event){
        if(event->button()==1)
            emit configurationClicked(this);
    }

    /** ----------------------------------------------------------------------
    * @brief setKeys(), adds the key items in QComboBox
    */
    void setKeys(){
        key->addItem("Up");
        key->addItem("Down");
        key->addItem("Right");
        key->addItem("Left");
        key->addItem("A");
        key->addItem("B");
        key->addItem("C");
        key->addItem("D");
        key->addItem("E");
        key->addItem("F");
        key->addItem("G");
        key->addItem("H");
        key->addItem("I");
        key->addItem("J");
        key->addItem("k");
        key->addItem("L");
        key->addItem("M");
        key->addItem("N");
        key->addItem("O");
        key->addItem("P");
        key->addItem("Q");
        key->addItem("R");
        key->addItem("S");
        key->addItem("T");
        key->addItem("U");
        key->addItem("V");
        key->addItem("W");
        key->addItem("x");
        key->addItem("Y");
        key->addItem("Z");


    }

};

/** ========================================================================
 * @brief ListConfiguration class is a class to manage all configuracionss
 */
class ListConfiguration{

public:

    Configuration *current;
    Configuration *h;

    /** ----------------------------------------------------------------------
    * @brief constructor, set the initial values
    */
    ListConfiguration(){
        h=NULL;
        current=NULL;
    }

    /** ----------------------------------------------------------------------
    * @brief destructor, Empty the list to optimize memory
    */
    ~ListConfiguration(){
        empty();
    }

    /** ----------------------------------------------------------------------
    * @brief add, add configuration to a list
    */
    void add(Configuration *c){
        if(!h) h=c;
        else{
            Configuration *aux=h;
            while(aux->next) aux=aux->next;
            aux->next=c;
            c->back=aux;
        }
    }

    /** ----------------------------------------------------------------------
    * @brief size, return the size of the list
    */
    int size(){
        int s=0;
        Configuration *aux=h;

        while(aux){
            s++;
            aux=aux->next;
        }

        return s;
    }

    /** ----------------------------------------------------------------------
    * @brief empty, Empty the list to optimize memory
    */
    void empty(){

        while(h){
            Configuration *aux=h;
            h=h->next;
            delete(aux);
        }
    }

    /** ----------------------------------------------------------------------
    * @brief changeColor, edge changes the settings to black
    */
    void changeColor(){

        Configuration *aux=h;
        while(aux){
            aux->chageColor(0);
            aux=aux->next;
        }
    }

    /** ----------------------------------------------------------------------
    * @brief remove, remove a configuration form the list
    */
    void remove(Configuration *p){

        if(p == h){
            Configuration *aux2=h;
            h = h->next;

            if(h)
                h->back = NULL;

            delete(aux2);
        }
        else{
            Configuration *aux = h;
            while(aux){
                if(aux == p){
                    Configuration *aux2=aux;
                    aux->back->next = aux->next;
                    if(aux->next)
                        aux->next->back = aux->back;
                    delete(aux2);
                }
                aux = aux->next;
            }
        }
    }

    /** ----------------------------------------------------------------------
    * @brief get, get a configuration form a position
    */
    Configuration *get(int p){
        Configuration *aux=h;
        int counter=0;
        while(aux){
            if(counter==p)	return aux;
            aux=aux->next;
            counter++;
        }

        return NULL;
    }

    /** ----------------------------------------------------------------------
    * @brief paint, sets the geometry of the settings in the window
    */
    void paint(){

        int s=1;
        Configuration *aux=h;
        while(aux){
            aux->setGeometry(20,50*s-20,250,50);
            s++;
            aux=aux->next;
        }
    }

    /** ----------------------------------------------------------------------
    * @brief searchConfiguration, looking for a configuration according to their key
    */
    Configuration *searchConfiguration(int k){
        Configuration *aux=h;
        while(aux){
            if(aux->k==k) return aux;
            aux=aux->next;
        }

        return NULL;
    }


};


#endif // CONFIGURATION_H
