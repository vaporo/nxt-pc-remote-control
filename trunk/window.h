#ifndef WINDOW_H
#define WINDOW_H

#include <QFrame>
#include <QPushButton>
#include <QLabel>
#include <QComboBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QKeyEvent>
#include <QProgressBar>
#include <QMenu>
#include <QFile>
#include <QThread>
#include <QGroupBox>
#include <QComboBox>
#include <configuration.h>
#include <QMessageBox>
#include <QTextEdit>
#include <QCheckBox>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenuBar>
#include <QFileDialog>


#include <network.h>
#include <idiom.h>

#define len(x) sizeof(x)/sizeof(byte)
#define non(x) (byte)-(x)

/** ========================================================================
* @brief MyButton class overload to QPushButton due to, was necesary do
* programming with especial events.
*/
class MyButton : public QPushButton {
Q_OBJECT
public:

    /** ----------------------------------------------------------------------
    * @brief MyButton constructor transport label to superclass
    */
    MyButton(QString label) : QPushButton(label) {}
    MyButton(QString label,QFrame *f) : QPushButton(label,f) {}
    MyButton(QString label,QMainWindow *f) : QPushButton(label,f) {}

signals:

    /** ----------------------------------------------------------------------
    * @brief This is an abstract method to rightClick() programming event
    */
    void rightClick(QPoint);

protected:

    /** ----------------------------------------------------------------------
    * @brief keyPressEvent create a keyboard press event
    */
    void keyPressEvent(QKeyEvent *ev) {
        ev->ignore();
    }

    /** ----------------------------------------------------------------------
    * @brief mousePressEvent create a mouse click event
    */
    void mousePressEvent(QMouseEvent *ev) {
        if (ev->button() == Qt::RightButton) {
            emit rightClick(ev->pos());
        }
        else if (ev->button() == Qt::LeftButton) {
            emit clicked();
        }
    }
};


/** ========================================================================
 * @brief Thread class is implemented for fix disable GUI programmed in
 * another release of this applications.
 */
class Thread : public QThread {
    Q_OBJECT
private:

    QComboBox* devices;
    Network* net;
    int operation;

signals:

    /** ----------------------------------------------------------------------
    * @brief Creating events to after running.
    */
    void scanPerformed(int);
    void connectPerformed(bool);

public:
    /** ----------------------------------------------------------------------
    * @brief Thread constructor receive device combo and network references
    * to allow work with them.
    */
    Thread(QComboBox* combo,Network* n,int op):devices(combo), net(n), operation(op){}

    /** ----------------------------------------------------------------------
    * @brief run method has actions to exec in thread.
    */
    void run() {
        switch (operation) {
            case 1:
                try {
                    QStringList s = net->scanDevices();
                    devices->clear();
                    devices->addItems(s);
                    emit scanPerformed(0);
                }
                catch(int e) {
                    devices->clear();
                    emit scanPerformed(e);
                }
                break;
            case 2:
                bool state = net->bind(devices->currentText().left(17));
                emit connectPerformed(state);
                break;
        }
    }
};

/** ========================================================================
 * @brief Window class is the main class en this project, because has the
 * GUI functions to intercommunicate two actors.  NXT PC Remote Control and
 * LEGO gameer.
 */
class Window : public QMainWindow {
    Q_OBJECT

private:

    byte                power;
    MyButton            *scan,*bind;
    QComboBox           *devices;
    QLabel              *info;
    Network             *net;
    QMenu               *recents,*selectidiom;
    Idiom               idiom;
    Thread              *t;
    MyButton            *add;
    QGroupBox           *gMotorA;
    QGroupBox           *gMotorB;
    QGroupBox           *gMotorC;
    QGroupBox           *gMotorsInUse;
    QGroupBox           *gConfigurations;
    MyButton            *removeConfiguration;
    MyButton            *saveConfiguration;
    ListConfiguration   *sConfiguration;
    QLabel             *lNa[12];
    QCheckBox           *AState;
    QCheckBox           *BState;
    QCheckBox           *CState;
    QSlider             *AVel;
    QSlider             *BVel;
    QSlider             *CVel;
    QComboBox           *ADir;
    QComboBox           *BDir;
    QComboBox           *CDir;
    QMenu               *menu;
    QMenu               *menuFile;


    /** ----------------------------------------------------------------------
    * @brief loadSettings method set de initial profiles to NXT PC Remote
    * Control using source file ".nxt-pc-remote-control.cfg"
    */
    void loadSettings() {
        QFile f(".nxt-pc-remote-control.cfg");
        f.open(QIODevice::ReadOnly);
        if (!f.isOpen()) {
            idiom.setIdiomType(ENG);
            return;
        }
        QString data = f.readLine().data();
        idiom.setIdiomType(data=="eng\n"?ENG:SPA);
        QString data2 = f.readLine();
        int recentsCount = data2.toInt();
        for (int i=0; i<recentsCount; i++) {
            data = f.readLine().data();
            recents->addAction(data);
        }
        refreshIdiom();
        f.close();
    }

    /** ----------------------------------------------------------------------
    * @brief addRecent method admin de cache of connections worked
    */
    void addRecent(QString data) {
        data+="\n";
        foreach (QAction* action, recents->actions()) {
            if (action->text() == data)return;
        }
        recents->addAction(data);
    }

    /** ----------------------------------------------------------------------
    * @brief saveSettings save current application settings into a file
    */
    void saveSettings() {
        QFile f(".nxt-pc-remote-control.cfg");
        f.open(QIODevice::WriteOnly);
        if (!f.isOpen()) return;
        f.write( idiom.getIdiomType()==ENG?"eng\n":"spa\n" );
        char size[4];
        sprintf( size,"%d\n", recents->actions().length() );
        f.write( size );
        foreach (QAction* action, recents->actions()) {
            f.write( action->text().toStdString().c_str() );
        }
        f.close();
    }

    /** ----------------------------------------------------------------------
    * @brief refreshIdiom method update idiom of application.
    */
    void refreshIdiom() {
        setWindowTitle(idiom.getWindowTitle());
        scan->setText(idiom.getScanButtonLabel());
        scan->isEnabled() ? bind->setText(idiom.getConnectButtonLabel()) :
        bind->setText(idiom.getDisconnectButtonLabel());
        info->setPixmap(QPixmap(idiom.getImageInfo()));
        selectidiom->actions().at(0)->setText(idiom.getMenuEnglish());
        selectidiom->actions().at(1)->setText(idiom.getMenuSpanish());
        menu->actions().at(0)->setText(idiom.getMenuSelectIdiom());
        menu->actions().at(2)->setText(idiom.getMenuRecentConnections());
        menu->actions().at(4)->setText(idiom.getMenuClearConnections());
        menu->actions().at(6)->setText(idiom.getMenuAbout());
        menu->actions().at(8)->setText(idiom.getLoadFile());
        menu->actions().at(10)->setText(idiom.getSaveFile());
        menu->setTitle(idiom.getOptions());

        gMotorsInUse->setTitle(idiom.getMotorsInUse());
        AState->setText(idiom.getActive());
        BState->setText(idiom.getActive());
        CState->setText(idiom.getActive());
        gConfigurations->setTitle(idiom.getConfigurations());
        add->setText(idiom.getAdd());
        removeConfiguration->setText(idiom.getRemove());
        saveConfiguration->setText(idiom.getSave());

        ADir->clear();
        ADir->addItem(idiom.getLeft());
        ADir->addItem(idiom.getRight());
        BDir->clear();
        BDir->addItem(idiom.getLeft());
        BDir->addItem(idiom.getRight());
        CDir->clear();
        CDir->addItem(idiom.getLeft());
        CDir->addItem(idiom.getRight());
    }


public:

    /** ----------------------------------------------------------------------
    * @brief Window constructor launch application saving information in its
    * attribute, additionally, update GUI presentation.
    */
    Window(): power(0x55){
        setWindowTitle(idiom.getWindowTitle());
        resize(250,100);

        scan                    = new MyButton(idiom.getScanButtonLabel(),this);
        devices                 = new QComboBox(this);
        bind                    = new MyButton(idiom.getConnectButtonLabel(),this);
        info                    = new QLabel();
        add                     = new MyButton(idiom.getAdd(),this);
        gMotorA                 = new QGroupBox("Motor A",this);
        gMotorB                 = new QGroupBox("Motor B",this);
        gMotorC                 = new QGroupBox("Motor C",this);
        gMotorsInUse            = new QGroupBox(idiom.getMotorsInUse(),this);
        gConfigurations         = new QGroupBox(idiom.getConfigurations(),this);
        removeConfiguration     = new MyButton(idiom.getRemove(),this);
        saveConfiguration       = new MyButton(idiom.getSave(),this);
        sConfiguration          = new ListConfiguration();
        AState                  = new QCheckBox(idiom.getActive(),gMotorA);
        BState                  = new QCheckBox(idiom.getActive(),gMotorB);
        CState                  = new QCheckBox(idiom.getActive(),gMotorC);
        AVel                    = new QSlider(gMotorA);
        BVel                    = new QSlider(gMotorB);
        CVel                    = new QSlider(gMotorC);
        ADir                    = new QComboBox(gMotorA);
        BDir                    = new QComboBox(gMotorB);
        CDir                    = new QComboBox(gMotorC);

        info                    -> setVisible(false);
        add                     -> setGeometry(320,650,87,27);
        scan                    -> setGeometry(30,30,87,27);
        devices                 -> setGeometry(30,60,281,25);
        bind                    -> setGeometry(220,30,87,27);
        gMotorsInUse            -> setGeometry(30,100,261,111);
        gMotorA                 -> setGeometry(30,240,281,131);
        gMotorB                 -> setGeometry(30,390,281,131);
        gMotorC                 -> setGeometry(30,540,281,131);
        gConfigurations         -> setGeometry(320,20,300,600);
        gConfigurations         -> setStyleSheet("QGroupBox {border: 1px solid gray;border-radius: 9px;margin-top: 0.5em;}");
        removeConfiguration     -> setGeometry(420,650,87,27);
        saveConfiguration       -> setGeometry(520,650,87,27);
        AState                  -> setGeometry(20,30,91,21);
        BState                  -> setGeometry(20,30,91,21);
        CState                  -> setGeometry(20,30,91,21);

        ADir                    -> setGeometry(70,90,61,21);
        ADir                    -> addItem(idiom.getLeft());
        ADir                    -> addItem(idiom.getRight());
        BDir                    -> setGeometry(70,90,61,21);
        BDir                    -> addItem(idiom.getLeft());
        BDir                    -> addItem(idiom.getRight());
        CDir                    -> setGeometry(70,90,61,21);
        CDir                    -> addItem(idiom.getLeft());
        CDir                    -> addItem(idiom.getRight());

        AVel                    -> setOrientation(Qt::Horizontal);
        AVel                    -> setGeometry(70,60,120,24);
        AVel                    -> setRange(0x32,0x64);
        BVel                    -> setOrientation(Qt::Horizontal);
        BVel                    -> setGeometry(70,60,120,24);
        BVel                    -> setRange(0x32,0x64);
        CVel                    -> setOrientation(Qt::Horizontal);
        CVel                    -> setGeometry(70,60,120,24);
        CVel                    -> setRange(0x32,0x64);


        lNa[0]                  = new QLabel("Motor A",gMotorsInUse);
        lNa[0]                  -> setGeometry(30,30,58,15);

        lNa[1]                  = new QLabel("Motor B",gMotorsInUse);
        lNa[1]                  -> setGeometry(110,30,58,15);

        lNa[2]                  = new QLabel("Motor c",gMotorsInUse);
        lNa[2]                  -> setGeometry(190,30,58,15);

        lNa[3]                  = new QLabel(gMotorsInUse);
        lNa[3]                  -> setScaledContents(true);
        lNa[3]                  -> setPixmap(QPixmap(":/images/off.png"));
        lNa[3]                  -> setGeometry(30,60,31,31);

        lNa[4]                  = new QLabel(gMotorsInUse);
        lNa[4]                  -> setPixmap(QPixmap(":/images/off.png"));
        lNa[4]                  -> setGeometry(110,60,31,31);

        lNa[5]                  = new QLabel(gMotorsInUse);
        lNa[5]                  -> setPixmap(QPixmap(":/images/off.png"));
        lNa[5]                  -> setGeometry(190,60,31,31);

        lNa[6]                  = new QLabel("VEL",gMotorA);
        lNa[6]                  -> setGeometry(20,60,58,15);

        lNa[7]                  = new QLabel("VEL",gMotorB);
        lNa[7]                  -> setGeometry(20,60,58,15);

        lNa[8]                  = new QLabel("VEL",gMotorC);
        lNa[8]                  -> setGeometry(20,60,58,15);

        lNa[9]                  = new QLabel("DIR",gMotorA);
        lNa[9]                  -> setGeometry(20,90,21,16);

        lNa[10]                 = new QLabel("DIR",gMotorB);
        lNa[10]                 -> setGeometry(20,90,21,16);

        lNa[11]                 = new QLabel("DIR",gMotorC);
        lNa[11]                 -> setGeometry(20,90,21,16);

        devices                 -> setEnabled(false);
        bind                    -> setEnabled(false);
        info                    -> setPixmap(QPixmap(idiom.getImageInfo()));

        setFixedSize(630,700);
        recents                 = new QMenu(idiom.getMenuRecentConnections());
        selectidiom             = new QMenu(idiom.getMenuSelectIdiom());
        selectidiom             -> addAction(idiom.getMenuEnglish());
        selectidiom             -> addAction(idiom.getMenuSpanish());

        QAction *openFile   =new QAction("Open File",this);
        QAction *saveFile   =new QAction("Save File",this);
        menu                = menuBar()->addMenu(idiom.getOptions());
        menu                -> addMenu(selectidiom);
        menu                ->addSeparator();
        menu                -> addMenu(recents);
        menu                ->addSeparator();
        menu                -> addAction(idiom.getMenuClearConnections());
        menu                ->addSeparator();
        menu                -> addAction(idiom.getMenuAbout());
        menu                ->addSeparator();
        menu                -> addAction(openFile);
        menu                ->addSeparator();
        menu                -> addAction(saveFile);

        loadSettings();
        net                     = new Network();

        connect(scan,SIGNAL(clicked()),this,SLOT(scanDevices()));
        connect(bind,SIGNAL(clicked()),this,SLOT(connectDevice()));
        connect(menu,SIGNAL(triggered(QAction*)),this,SLOT(menuOption(QAction*)));
        connect(recents,SIGNAL(triggered(QAction*)),this,SLOT(recentSelection(QAction*)));
        connect(selectidiom,SIGNAL(triggered(QAction*)),this,SLOT(changeIdiom(QAction*)));
        connect(add,SIGNAL(clicked()),this,SLOT(addConfiguration()));
        connect(removeConfiguration,SIGNAL(clicked()),this,SLOT(mRemoveConfiguration()));
        connect(saveConfiguration,SIGNAL(clicked()),this,SLOT(mSaveConfiguration()));
        connect(saveFile,SIGNAL(triggered()),this,SLOT(saveFile()));
        connect(openFile,SIGNAL(triggered()),this,SLOT(loadFile()));

    }

    /** ----------------------------------------------------------------------
    * @brief Window destructor to exec saving settings and clear memory
    */
    ~Window() {
        saveSettings();
        delete net;
        sConfiguration->empty();
    }

protected:

    /** ----------------------------------------------------------------------
    * @brief keyPressEvent exec commands when user is play with NXT PC
    * Remote Control.
    */
    void keyPressEvent(QKeyEvent *event) {

        if (bind->text() == idiom.connectButtonLabel) return;
        if (!event->isAutoRepeat()) {
            Configuration *aux=sConfiguration->searchConfiguration(event->key());
            if(aux){
                aux->run(net);
                if(aux->motorA->state)
                    lNa[3] -> setPixmap(QPixmap(":/images/on.png"));
                else
                    lNa[3] -> setPixmap(QPixmap(":/images/off.png"));

                if(aux->motorB->state)
                    lNa[4] -> setPixmap(QPixmap(":/images/on.png"));
                else
                    lNa[4] -> setPixmap(QPixmap(":/images/off.png"));

                if(aux->motorC->state)
                    lNa[5] -> setPixmap(QPixmap(":/images/on.png"));
                else
                    lNa[5] -> setPixmap(QPixmap(":/images/off.png"));
            }

        }
    }

    /** ----------------------------------------------------------------------
    * @brief keyReleaseEvent leave commands when user is play with NXT PC
    * Remote Control.
    */
    void keyReleaseEvent(QKeyEvent *event) {
        if (bind->text() == idiom.getConnectButtonLabel()) return;

        if (!event->isAutoRepeat()) {

            byte bytes0[] = { 0x04, 0x00, power, 0x02, 0x01, 0x00, 0x20, 0x00,
            0x00, 0x00, 0x00 };
            net->directCommand(bytes0, len(bytes0));

            byte bytes1[] = { 0x04, 0x01, power, 0x02, 0x01, 0x00, 0x20, 0x00,
            0x00, 0x00, 0x00 };
            net->directCommand(bytes1, len(bytes1));

            byte bytes2[] = { 0x04, 0x02, power, 0x02, 0x01, 0x00, 0x20, 0x00,
            0x00, 0x00, 0x00 };
            net->directCommand(bytes2, len(bytes2));

            lNa[3] -> setPixmap(QPixmap(":/images/off.png"));
            lNa[4] -> setPixmap(QPixmap(":/images/off.png"));
            lNa[5] -> setPixmap(QPixmap(":/images/off.png"));

        }
    }

    /** ----------------------------------------------------------------------
    * @brief addConfiguration add configuration from file
    */
    void addConfiguration(QString d){
        QStringList con=d.split(";");
        QString name=con.at(0);
        Configuration *aux=new Configuration(name,gConfigurations);
        sConfiguration->current=aux;
        sConfiguration->changeColor();
        aux->chageColor(1);
        connect(aux,SIGNAL(configurationClicked(Configuration*)),this,SLOT(getConfiguration(Configuration *)));
        sConfiguration->add(aux);
        sConfiguration->paint();
        aux->setVisible(true);

        aux->k=con.at(1).toInt();
        aux->changeKey(con.at(1).toInt());

        QString motorA=con.at(2);
        QStringList mA=motorA.split(",");
        aux->motorA->state=mA.at(0).toInt();
        aux->motorA->dir=mA.at(1).toInt();
        aux->motorA->vel=mA.at(2).toInt();

        QString motorB=con.at(3);
        QStringList mB=motorB.split(",");
        aux->motorB->state=mB.at(0).toInt();
        aux->motorB->dir=mB.at(1).toInt();
        aux->motorB->vel=mB.at(2).toInt();

        QString motorC=con.at(4);
        QStringList mC=motorC.split(",");
        aux->motorC->state=mC.at(0).toInt();
        aux->motorC->dir=mC.at(1).toInt();
        aux->motorC->vel=mC.at(2).toInt();

    }

public slots:

    /** ----------------------------------------------------------------------
    * @brief saveFile save Configuration into a file
    */

    void saveFile(){
        QString fn = QFileDialog::getSaveFileName(this, idiom.getSaveFile(),
                                                  tr("n.nxt"), tr("NXT files (*.nxt)"));
        if(!fn.isEmpty()){
            QFile f(fn);
            f.open(QIODevice::WriteOnly);
            if (!f.isOpen()) return;
            f.write("NXT\n");
            char size[4];
            sprintf( size,"%d\n",sConfiguration->size());
            f.write(size);

            for(int i=0;i<sConfiguration->size();i++){
                Configuration *p=sConfiguration->get(i);
                QString q=p->title()+";"+QString::number(p->k)+";"
                                        +QString::number(p->motorA->state)+","+QString::number(p->motorA->dir)+","+QString::number(p->motorA->vel)+";"
                                        +QString::number(p->motorB->state)+","+QString::number(p->motorB->dir)+","+QString::number(p->motorB->vel)+";"
                                        +QString::number(p->motorC->state)+","+QString::number(p->motorC->dir)+","+QString::number(p->motorC->vel)+ "\n";



                f.write(q.toStdString().c_str());
            }

            f.close();
        }
    }

    /** ----------------------------------------------------------------------
    * @brief LoadFile load Configurations from a file
    */
    void loadFile(){
        QString fn=QFileDialog::getOpenFileName( this,
                                                 idiom.getLoadFile(),
                                                 QDir::currentPath(),
                                                 tr("NXT File (*.nxt)"),
                                                 0, QFileDialog::DontUseNativeDialog );

        if(!fn.isEmpty()){
            QFile f(fn);
            f.open(QIODevice::ReadOnly);
            if (!f.isOpen())return;

            QString data = f.readLine();
            if(data!="NXT\n")return;
            sConfiguration->empty();
            data=f.readLine();
            int c=data.toInt();
            for(int i=0;i<c;i++)
                addConfiguration(f.readLine().data());
            f.close();
        }

    }


    /** ----------------------------------------------------------------------
    * @brief saveConfiguration method save the configuration
    * into a motors
    */
    void mSaveConfiguration(){
        if(!sConfiguration->current)return;

        if(ADir->currentIndex()==0)sConfiguration->current->motorA->dir=0;
        else sConfiguration->current->motorA->dir=1;

        if(BDir->currentIndex()==0)sConfiguration->current->motorB->dir=0;
        else sConfiguration->current->motorB->dir=1;

        if(CDir->currentIndex()==0)sConfiguration->current->motorC->dir=0;
        else sConfiguration->current->motorC->dir=1;

        sConfiguration->current->k=sConfiguration->current->getKey();

        if(AState->isChecked()) sConfiguration->current->motorA->state=true;
        else sConfiguration->current->motorA->state=false;
        if(BState->isChecked()) sConfiguration->current->motorB->state=true;
        else sConfiguration->current->motorB->state=false;
        if(CState->isChecked()) sConfiguration->current->motorC->state=true;
        else sConfiguration->current->motorC->state=false;

        sConfiguration->current->motorA->vel=AVel->value();
        sConfiguration->current->motorB->vel=BVel->value();
        sConfiguration->current->motorC->vel=CVel->value();

    }

    /** ----------------------------------------------------------------------
    * @brief removeConfiguration method remove configuration
    */
    void mRemoveConfiguration(){
        if(sConfiguration->current){
            sConfiguration->remove(sConfiguration->current);
            sConfiguration->paint();
        }
    }

    /** ----------------------------------------------------------------------
    * @brief configuration method get the configuration
    */
    void getConfiguration(Configuration *s){
        sConfiguration->changeColor();
        sConfiguration->current=s;
        s->chageColor(1);

        AState->setChecked(s->motorA->state);
        BState->setChecked(s->motorB->state);
        CState->setChecked(s->motorC->state);

        AVel->setValue(s->motorA->vel);
        BVel->setValue(s->motorB->vel);
        CVel->setValue(s->motorC->vel);

        ADir->setCurrentIndex(s->motorA->dir);
        BDir->setCurrentIndex(s->motorB->dir);
        CDir->setCurrentIndex(s->motorC->dir);

    }

    /** ----------------------------------------------------------------------
    * @brief addConfiguration method add new configuration
    */
    void addConfiguration(){

        if(sConfiguration->size()>=11)return;

        QMessageBox *msgBox=new QMessageBox(this);
        msgBox->setText("Name: ");
        QLineEdit *p=new QLineEdit(msgBox);

        p->setGeometry(80,10,100,20);
        msgBox->setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        msgBox->setDefaultButton(QMessageBox::Ok);

        Configuration *d;
        int v=msgBox->exec();

        switch (v) {
            case QMessageBox::Ok:
                if(p->text().length()>0){
                    d=new Configuration(p->text(),gConfigurations);
                    sConfiguration->current=d;
                    sConfiguration->changeColor();
                    d->chageColor(1);
                    connect(d,SIGNAL(configurationClicked(Configuration*)),this,SLOT(getConfiguration(Configuration *)));
                    sConfiguration->add(d);
                    sConfiguration->paint();
                    d->setVisible(true);
                }
                break;
        }

        delete(p);
        delete(msgBox);
    }

    /** ----------------------------------------------------------------------
    * @brief scanDevices method search all devices with bluetooth
    * functionality.
    */
    void scanDevices() {
        devices->clear();
        devices->setEnabled(false);
        bind->setEnabled(false);
        scan->setEnabled(false);
        devices->addItem(idiom.getMessageSearching());

        t = new Thread(devices,net,1);
        connect(t,SIGNAL(scanPerformed(int)),this,SLOT(scanPerformed(int)));
        t->start();
    }

    /** ----------------------------------------------------------------------
    * @brief scanPerformed is run after net scan->
    */
    void scanPerformed(int throwstate) {
        devices->setEnabled(true);
        scan->setEnabled(true);
        if (throwstate==0) {
            bind->setEnabled(true);
        }
        else {
            bind->setEnabled(false);
            switch(throwstate) {
                case 1: {
                    devices->addItem(idiom.getMessageBluetoothDisabled());
                    break;
                }
                case 2: {
                    devices->addItem(idiom.getMessageNearDivices());
                    break;
                }
            }
        }
        delete t;
    }

    /** ----------------------------------------------------------------------
    * @brief connectDevice bind NXT PC Remote Control with a wanted device
    */
    void connectDevice() {
        if (bind->text() == idiom.getConnectButtonLabel()) {
            bind->setEnabled(false);
            scan->setEnabled(false);
            devices->setEnabled(false);
            t = new Thread(devices,net,2);
            connect(t,SIGNAL(connectPerformed(bool)),this,SLOT(connectPerformed(bool)));
            t->start();
        }
        else {
            net->unbind();
            scan->setEnabled(true);
            devices->setEnabled(true);
            bind->setText(idiom.getConnectButtonLabel());
            recents->setEnabled(true);
            for (int i=0; i<6;i++) menu->actions().at(i)->setEnabled(true);
        }
    }

    /** ----------------------------------------------------------------------
    * @brief connectPerfomred is run after net bind function.
    */
    void connectPerformed(bool ok) {
        if (ok) {
            bind->setText(idiom.getDisconnectButtonLabel());
            addRecent(devices->currentText());
            for (int i=0; i<6;i++) menu->actions().at(i)->setEnabled(false);
        }
        else {
            scan->setEnabled(true);
            devices->clear();
            devices->addItem(idiom.getMessageDeviceAvailable());
            devices->setEnabled(true);
        }
        bind->setEnabled(true);
        delete t;
    }

    /** ----------------------------------------------------------------------
    * @brief recentSelection method short way, in the connection event.  It
    * method is exec when an user use the cache connections.
    */
    void recentSelection(QAction* action) {
        bind->setEnabled(false);
        scan->setEnabled(false);
        devices->clear();
        devices->addItem(action->text().left(action->text().size()-1));
        devices->setEnabled(false);
        t = new Thread(devices,net,2);
        connect(t,SIGNAL(connectPerformed(bool)),this,SLOT(connectPerformed(bool)));
        t->start();
    }

    /** ----------------------------------------------------------------------
    * @brief changeIdiom method switch de interface language between Englis
    * and Spanish
    */
    void changeIdiom(QAction *action) {
        if (action->text().left(3)==idiom.getMenuEnglish().left(3) && idiom.getIdiomType()!=ENG) {
            idiom.setIdiomType(ENG);
            refreshIdiom();
        }
        else if (action->text().left(3)==idiom.getMenuSpanish().left(3) && idiom.getIdiomType()!=SPA) {
            idiom.setIdiomType(SPA);
            refreshIdiom();
        }
    }

    /** ----------------------------------------------------------------------
    * @brief menuOption method exec two of all additional options: Show about,
    * and Clear cache connections
    */
    void menuOption(QAction* action) {

        if (action->text()==idiom.getMenuAbout()) {
            showAbout(true);
        }
        else if (action->text()==idiom.getMenuClearConnections()) {
            recents->clear();
        }
    }

    /** ----------------------------------------------------------------------
    * @brief showAbout method, show the author information.
    */
    void showAbout(bool state) {
        if (state==true) {
            //info->setPixmap(QPixmap(":/images/about.png"));
            //info->setVisible(true);
            QWidget *w=new QWidget();

            w->resize(300,500);
            QLabel *logo=new QLabel(w);
            logo->setGeometry(85,40,130,130);
            logo->setPixmap(QPixmap(":/images/logo.gif"));
            logo->setScaledContents(true);
            QWidget *text[8];
            text[0]=new QLabel("Copyright",w);
            text[0]->setGeometry(120,220,100,20);

            text[1]=new QLabel("www.udenar.edu.co/sonar",w);
            text[1]->setGeometry(80,240,300,20);

            text[2]=new QLabel(QObject::trUtf8("Universidad De Nariño"),w);
            text[2]->setGeometry(90,260,300,20);

            text[3]=new QLabel("Pasto - Colombia",w);
            text[3]->setGeometry(110,280,300,20);

            text[4]=new QLabel("GonzaloHernandez@udenar.edu.co",w);
            text[4]->setGeometry(50,340,300,20);

            text[5]=new QLabel("Insuasty@udenar.edu.co",w);
            text[5]->setGeometry(80,360,300,20);

            text[6]=new QLabel("jg@udenar.edu.co",w);
            text[6]->setGeometry(100,380,300,20);

            text[7]=new QLabel("_______________________________________",w);
            text[7]->setGeometry(30,300,300,20);

            text[7]=new QLabel("_______________________________________",w);
            text[7]->setGeometry(30,400,300,20);

            text[6]=new QLabel("felipero25@gmail.com",w);
            text[6]->setGeometry(80,420,300,20);
            w->setVisible(true);
        }
    }
};

#endif // WINDOW_H
