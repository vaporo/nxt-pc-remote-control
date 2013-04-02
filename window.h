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

#define length(x) sizeof(x)/sizeof(byte)
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

class Window : public QFrame {
  Q_OBJECT
private:
  byte        power;
  bool        lowswitch;
  byte        powerlow;
  //-----------------------
  MyButton      *scan,*bind;
  QComboBox     *devices;
  QLabel        *info;
  Network       *net;
  QProgressBar  *lowspeed,*highspeed;
  QMenu         *menu;
  QMenu         *recents;

  QStringList loadRecents() {
    QStringList list;
    QFile f("nxt-pc-remote-contro.cfg");
    f.open(QIODevice::ReadOnly);
    if (!f.isOpen()) return list;
    QString data = f.readLine().data();
    while (!data.isEmpty()) {
      list.append(data);
      data = f.readLine().data();
    }

    f.close();
    return list;
  }

  void addRecent(QString data) {
    data+="\n";
    foreach (QAction* action, recents->actions()) {
      if (action->text() == data)return;
    }
    recents->addAction(data);
  }

  void saveRecents() {
    QFile f("nxt-pc-remote-contro.cfg");
    f.open(QIODevice::WriteOnly);
    if (!f.isOpen()) return;
    foreach (QAction* action, recents->actions()) {
      f.write( action->text().toStdString().c_str() );
    }
    f.close();
  }

public:
  //-----------------------------------------------------------------------
  Window(): power(0x55), lowswitch(false), powerlow(0x3E) {
    setWindowTitle("NXT PC Remote Control");
    resize(250,100);

    scan      = new MyButton("Buscar");
    devices   = new QComboBox();
    bind      = new MyButton("Conectar");
    info      = new QLabel();
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
    info->setPixmap(QPixmap(":/images/info.png"));
    setStyleSheet("QFrame{background-color:white}");
    lowspeed->setMinimum(0x32);
    lowspeed->setMaximum(0x64);
    highspeed->setMinimum(0x32);
    highspeed->setMaximum(0x64);
    lowspeed->setValue(powerlow);
    highspeed->setValue(power);

    setFixedSize(278,438);
    recents = new QMenu("Conexiones Recientes");
    menu->addMenu(recents);
    QStringList list = loadRecents();
    foreach (QString data, list) {
       recents->addAction(data);
    }

    net = new Network();

    connect(scan,SIGNAL(clicked()),this,SLOT(scanDevices()));
    connect(bind,SIGNAL(clicked()),this,SLOT(connectDevice()));
    connect(scan,SIGNAL(rightClick(QPoint)),this,SLOT(recentConnections(QPoint)));
    connect(menu,SIGNAL(triggered(QAction*)),this,SLOT(recentSelection(QAction*)));
  }

  //-----------------------------------------------------------------------
  ~Window() {
    saveRecents();
    delete net;
  }

protected:
  void keyPressEvent(QKeyEvent *event) {
    if (bind->text() == "Conectar") return;
    if (!event->isAutoRepeat()) {
      switch (event->key()) {

        case Qt::Key_Alt: {
          lowswitch = true;
          break;
        }

        case Qt::Key_B: {
          byte bytes[] = { 0x03, 0x0B, 0x02, 0xF4, 0x01 };
          Telegram t;
          t.append(bytes, length(bytes));
          net->directCommand(t);
          break;
        }

        case Qt::Key_Up : {
          byte bytes3[] = { 0x04, 0x01, lowswitch?powerlow:power, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, length(bytes3));

          byte bytes4[] = { 0x04, 0x02, lowswitch?powerlow:power, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes4, length(bytes4));
          break;
        }

        case Qt::Key_Down : {
          byte bytes3[] = { 0x04, 0x01, lowswitch?non(powerlow):non(power), 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, length(bytes3));

          byte bytes4[] = { 0x04, 0x02, lowswitch?non(powerlow):non(power), 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes4, length(bytes4));
          break;
        }

        case Qt::Key_Left : {
          byte bytes3[] = { 0x04, 0x01, lowswitch?non(powerlow):non(power), 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, length(bytes3));

          byte bytes4[] = { 0x04, 0x02, lowswitch?powerlow:power, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes4, length(bytes4));
          break;
        }

        case Qt::Key_Right : {
          byte bytes3[] = { 0x04, 0x01, lowswitch?powerlow:power, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, length(bytes3));

          byte bytes4[] = { 0x04, 0x02, lowswitch?non(powerlow):non(power), 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes4, length(bytes4));
          break;
        }

        case Qt::Key_N : {
          byte bytes3[] = { 0x04, 0x00, lowswitch?powerlow:power, 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, length(bytes3));
          break;
        }

        case Qt::Key_M : {
          byte bytes3[] = { 0x04, 0x00, lowswitch?non(powerlow):non(power), 0x01, 0x00, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes3, length(bytes3));
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
    if (bind->text() == "Conectar") return;
    if (!event->isAutoRepeat()) {
      switch (event->key()) {
        case Qt::Key_Up:
        case Qt::Key_Down:
        case Qt::Key_Left:
        case Qt::Key_Right:
        case Qt::Key_N:
        case Qt::Key_M: {
          byte bytes0[] = { 0x04, 0x00, power, 0x02, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes0, length(bytes0));

          byte bytes1[] = { 0x04, 0x01, power, 0x02, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes1, length(bytes1));

          byte bytes2[] = { 0x04, 0x02, power, 0x02, 0x01, 0x00, 0x20, 0x00, 0x00, 0x00, 0x00 };
          net->directCommand(bytes2, length(bytes2));
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
      devices->addItem("Buscando Dispositivos...");
      info->setPixmap(QPixmap(":/images/clock.png"));
      setEnabled(false);
      repaint();
      devices->clear();
      devices->addItems(net->scanDevices());
      info->setPixmap(QPixmap(":/images/info.png"));
      devices->setEnabled(true);
      bind->setEnabled(true);
      setEnabled(true);
    }
    catch(int e) {
      switch(e) {
      case 1: info->setText("Bluetooth deshabilitado"); break;
      case 2: devices->clear(); devices->addItem("No hay dispositivos cercanos"); break;
      }
      info->setPixmap(QPixmap(":/images/info.png"));
      setEnabled(true);
    }
  }

  //-----------------------------------------------------------------------
  void connectDevice() {
    if (bind->text() == "Conectar") {
      info->setPixmap(QPixmap(":/images/clock.png"));
      setEnabled(false);
      repaint();
      net->bind(devices->currentText().left(17));
      info->setPixmap(QPixmap(":/images/info.png"));
      scan->setEnabled(false);
      devices->setEnabled(false);
      bind->setText("Desconectar");
      setEnabled(true);
      addRecent(devices->currentText());
    }
    else {
      net->unbind();
      scan->setEnabled(true);
      devices->setEnabled(true);
      bind->setText("Conectar");
    }
  }

  //-----------------------------------------------------------------------
  void recentConnections(QPoint point) {
    menu->exec(mapToGlobal(point));
  }

  //-----------------------------------------------------------------------
  void recentSelection(QAction* action) {
    info->setPixmap(QPixmap(":/images/clock.png"));
    setEnabled(false);
    repaint();
    net->bind(action->text().left(17));
    info->setPixmap(QPixmap(":/images/info.png"));
    scan->setEnabled(false);
    devices->clear();
    devices->addItem(action->text());
    devices->setEnabled(false);
    bind->setText("Desconectar");
    bind->setEnabled(true);
    setEnabled(true);
  }
};

#endif // WINDOW_H
