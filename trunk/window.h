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
#include <network.h>
#include <idiom.h>

#define len(x) sizeof(x)/sizeof(byte)
#define non(x) (byte)-(x)

//=========================================================================

class MyButton : public QPushButton {
  Q_OBJECT
public:
  MyButton(QString label) : QPushButton(label) {
  }
signals:
  void rightClick(QPoint);

protected:
  void keyPressEvent(QKeyEvent *ev) {
    ev->ignore();
  }
  void mousePressEvent(QMouseEvent *ev) {
    if (ev->button() == Qt::RightButton) {
      emit rightClick(ev->pos());
    }
    else if (ev->button() == Qt::LeftButton) {
      emit clicked();
    }
  }
};

//=========================================================================

class MyLabel : public QLabel {
  Q_OBJECT
signals:
  void rightClick(QPoint);
protected:
  void mousePressEvent(QMouseEvent *ev) {
    if (ev->button() == Qt::RightButton) {
      emit rightClick(ev->pos());
    }
    else {
      ev->ignore();
    }
  }
};

//=========================================================================

class Window : public QFrame {
  Q_OBJECT
private:
  byte        power;
  bool        lowswitch;
  byte        powerlow;
  //-----------------------
  MyButton      *scan,*bind;
  QComboBox     *devices;
  MyLabel       *info;
  Network       *net;
  QProgressBar  *lowspeed,*highspeed;
  QMenu         *menu;
  QMenu         *recents,*selectidiom;
  Idiom         idiom;

  void loadSettings() {
    QFile f(".nxt-pc-remote-contro.cfg");
    f.open(QIODevice::ReadOnly);
    if (!f.isOpen()) {
       idiom.setIdiomType(ENG);
       return;
    }
    QString data = f.readLine().data();
    idiom.setIdiomType(data=="eng\n"?ENG:SPA);
    int recentsCount = f.readLine().toInt();
    for (int i=0; i<recentsCount; i++) {
      data = f.readLine().data();
      recents->addAction(data);
    }
    refreshIdiom();
    f.close();
  }
//-----------------------------------------------------------------------
  void addRecent(QString data) {
    data+="\n";
    foreach (QAction* action, recents->actions()) {
      if (action->text() == data)return;
    }
    recents->addAction(data);
  }
   //-----------------------------------------------------------------------
  void saveSettings() {
    QFile f(".nxt-pc-remote-contro.cfg");
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

  //-----------------------------------------------------------------------
  void refreshIdiom() {
      setWindowTitle(idiom.getWindowTitle());
      scan->setText(idiom.getScanButtonLabel());
      scan->isEnabled() ? bind->setText(idiom.getConnectButtonLabel()) : bind->setText(idiom.getDisconnectButtonLabel());
      info->setPixmap(QPixmap(idiom.getImageInfo()));
      selectidiom->clear();
      selectidiom->addAction(idiom.getMenuEnglish());
      selectidiom->addAction(idiom.getMenuSpanish());
      menu->actions().at(0)->setText(idiom.getMenuRecentConnections());
      menu->actions().at(1)->setText(idiom.getMenuClearConnections());
      menu->actions().at(3)->setText(idiom.getMenuSelectIdiom());
      menu->actions().at(5)->setText(idiom.getMenuAbout());
  }

public:
  //-----------------------------------------------------------------------
  Window(): power(0x55), lowswitch(false), powerlow(0x3E) {
    setWindowTitle(idiom.getWindowTitle());
    resize(250,100);

    scan      = new MyButton(idiom.getScanButtonLabel());
    devices   = new QComboBox();
    bind      = new MyButton(idiom.getConnectButtonLabel());
    info      = new MyLabel();
    lowspeed  = new QProgressBar();
    highspeed = new QProgressBar();
    menu      = new QMenu();

    QHBoxLayout* buttons = new QHBoxLayout();
    QVBoxLayout* layout = new QVBoxLayout();

    setLayout(layout);
    buttons->addWidget(scan);
    buttons->addWidget(bind);
    layout->addLayout(buttons);
    layout->addWidget(devices);
    layout->addWidget(info);
    layout->addWidget(highspeed);
    layout->addWidget(lowspeed);

    devices->setEnabled(false);
    bind->setEnabled(false);
    info->setPixmap(QPixmap(idiom.getImageInfo()));
    setStyleSheet("QFrame{background-color:white}");
    lowspeed->setMinimum(0x32);
    lowspeed->setMaximum(0x64);
    highspeed->setMinimum(0x32);
    highspeed->setMaximum(0x64);
    lowspeed->setValue(powerlow);
    highspeed->setValue(power);

    setFixedSize(278,438);
    recents = new QMenu(idiom.getMenuRecentConnections());
    selectidiom = new QMenu(idiom.getMenuSelectIdiom());
    menu->addMenu(recents);
    menu->addAction(idiom.getMenuClearConnections());
    menu->addSeparator();
    menu->addMenu(selectidiom);
    menu->addSeparator();
    menu->addAction(idiom.getMenuAbout());
    selectidiom->addAction(idiom.getMenuEnglish());
    selectidiom->addAction(idiom.getMenuSpanish());

    loadSettings();
    net = new Network();

    connect(scan,SIGNAL(clicked()),this,SLOT(scanDevices()));
    connect(bind,SIGNAL(clicked()),this,SLOT(connectDevice()));
    connect(info,SIGNAL(rightClick(QPoint)),this,SLOT(popMenu(QPoint)));
    connect(menu,SIGNAL(triggered(QAction*)),this,SLOT(menuOption(QAction*)));
    connect(recents,SIGNAL(triggered(QAction*)),this,SLOT(recentSelection(QAction*)));
    connect(selectidiom,SIGNAL(triggered(QAction*)),this,SLOT(changeIdiom(QAction*)));
  }

  //-----------------------------------------------------------------------
  ~Window() {
    saveSettings();
    delete net;
  }

protected:
  void keyPressEvent(QKeyEvent *event) {
    if (bind->text() == idiom.connectButtonLabel) return;
    if (!event->isAutoRepeat()) {
      switch (event->key()) {

        case Qt::Key_Alt: {
          lowswitch = true;
          break;
        }

        case Qt::Key_B: {
          byte bytes[] = { 0x03, 0x0B, 0x02, 0xF4, 0x01 };
          Telegram t;
          t.append(bytes, len(bytes));
          net->directCommand(t);
          break;
        }

        case Qt::Key_Up : {
          byte bytes3[] = { 0x04, 0x01, lowswitch?powerlow:power, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, len(bytes3));

          byte bytes4[] = { 0x04, 0x02, lowswitch?powerlow:power, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes4, len(bytes4));
          break;
        }

        case Qt::Key_Down : {
          byte bytes3[] = { 0x04, 0x01, lowswitch?non(powerlow):non(power), 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, len(bytes3));

          byte bytes4[] = { 0x04, 0x02, lowswitch?non(powerlow):non(power), 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes4, len(bytes4));
          break;
        }

        case Qt::Key_Left : {
          byte bytes3[] = { 0x04, 0x01, lowswitch?non(powerlow):non(power), 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, len(bytes3));

          byte bytes4[] = { 0x04, 0x02, lowswitch?powerlow:power, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes4, len(bytes4));
          break;
        }

        case Qt::Key_Right : {
          byte bytes3[] = { 0x04, 0x01, lowswitch?powerlow:power, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, len(bytes3));

          byte bytes4[] = { 0x04, 0x02, lowswitch?non(powerlow):non(power), 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes4, len(bytes4));
          break;
        }

        case Qt::Key_N : {
          byte bytes3[] = { 0x04, 0x00, lowswitch?powerlow:power, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, len(bytes3));
          break;
        }

        case Qt::Key_M : {
          byte bytes3[] = { 0x04, 0x00, lowswitch?non(powerlow):non(power), 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, len(bytes3));
          break;
        }

        case Qt::Key_Minus : {
          if (lowswitch) {
            if (powerlow>0x32) powerlow--;
            lowspeed->setValue(powerlow);
          }
          else {
            if (power>0x32) power--;
            highspeed->setValue(power);
          }
          break;
        }

        case Qt::Key_Plus : {
          if (lowswitch) {
            if (powerlow<0x64) powerlow++;
            lowspeed->setValue(powerlow);
          }
          else {
            if (power<0x64) power++;
            highspeed->setValue(power);
          }
          break;
        }

      }
    }
    else {
      switch (event->key()) {
        case Qt::Key_Minus : {
          if (lowswitch) {
            if (powerlow>0x32) powerlow--;
            lowspeed->setValue(powerlow);
          }
          else
          {
            if (power>0x32) power--;
            highspeed->setValue(power);
          }
          break;
        }

        case Qt::Key_Plus : {
          if (lowswitch) {
            if (powerlow<0x64) powerlow++;
            lowspeed->setValue(powerlow);
          }
          else {
            if (power<0x64) power++;
            highspeed->setValue(power);
          }
          break;
        }
      }
    }
  }

  void keyReleaseEvent(QKeyEvent *event) {
    if (bind->text() == idiom.getConnectButtonLabel()) return;
    if (!event->isAutoRepeat()) {
      switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_N:
        case Qt::Key_M: {
          byte bytes0[] = { 0x04, 0x00, power, 0x02, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes0, len(bytes0));

          byte bytes1[] = { 0x04, 0x01, power, 0x02, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes1, len(bytes1));

          byte bytes2[] = { 0x04, 0x02, power, 0x02, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes2, len(bytes2));
          break;
        }

        case Qt::Key_Alt: {
          lowswitch = false;
          break;
        }

      }
    }
  }

public slots:
  //-----------------------------------------------------------------------
  void scanDevices() {
    try {
      devices->clear();
      devices->addItem(idiom.getMessageSearching());
      info->setPixmap(QPixmap(":/images/clock.png"));
      setEnabled(false);
      repaint();
      devices->clear();
      devices->addItems(net->scanDevices());
      info->setPixmap(QPixmap(idiom.getImageInfo()));
      devices->setEnabled(true);
      bind->setEnabled(true);
      setEnabled(true);
    }
    catch(int e) {
      switch(e) {
        case 1: {
          devices->addItem(idiom.getMessageBluetoothDisabled());
          break;
        }
        case 2: {
          devices->addItem(idiom.getMessageNearDivices());
          break;
        }
      }
      info->setPixmap(QPixmap(idiom.getImageInfo()));
      setEnabled(true);
    }
  }

  //-----------------------------------------------------------------------
  void connectDevice() {
    if (bind->text() == idiom.getConnectButtonLabel()) {
      info->setPixmap(QPixmap(":/images/clock.png"));
      setEnabled(false);
      repaint();
      net->bind(devices->currentText().left(17));
      info->setPixmap(QPixmap(idiom.getImageInfo()));
      scan->setEnabled(false);
      devices->setEnabled(false);
      bind->setText(idiom.getDisconnectButtonLabel());
      recents->setEnabled(false);
      setEnabled(true);
      addRecent(devices->currentText());
    }
    else {
      net->unbind();
      scan->setEnabled(true);
      devices->setEnabled(true);
      bind->setText(idiom.getConnectButtonLabel());
      recents->setEnabled(true);
    }
  }

  //-----------------------------------------------------------------------
  void popMenu(QPoint point) {
    menu->exec(info->mapToGlobal(point));
  }

  //-----------------------------------------------------------------------
  void recentSelection(QAction* action) {
    info->setPixmap(QPixmap(":/images/clock.png"));
    setEnabled(false);
    devices->clear();
    devices->addItem(action->text().left(action->text().size()-1));
    repaint();
    if (net->bind(action->text().left(17))) {
      scan->setEnabled(false);
      devices->setEnabled(false);
      bind->setText(idiom.getDisconnectButtonLabel());
      bind->setEnabled(true);
    }
    else {
      devices->clear();
      devices->addItem(idiom.getMessageDeviceAvailable());
      devices->setEnabled(false);
    }
    repaint();
    info->setPixmap(QPixmap(idiom.getImageInfo()));
    setEnabled(true);
  }

  //-----------------------------------------------------------------------

  void changeIdiom(QAction *action) {
     if (action->text()==idiom.getMenuEnglish() && idiom.getIdiomType()!=ENG) {
       idiom.setIdiomType(ENG);
       refreshIdiom();
     }
     else if (action->text()==idiom.getMenuSpanish() && idiom.getIdiomType()!=SPA) {
       idiom.setIdiomType(SPA);
       refreshIdiom();
     }
  }

  //-----------------------------------------------------------------------
  void menuOption(QAction* action) {
    if (action->text()==idiom.getMenuAbout()) {
      std::cout << action->text().toStdString() << std::endl;
    }
    else if (action->text()==idiom.getMenuClearConnections()) {
      recents->clear();
    }
  }
};

#endif // WINDOW_H
