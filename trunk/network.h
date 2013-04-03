#ifndef NETWORK_H
#define NETWORK_H

typedef unsigned char byte;

#include <QStringList>
#include <bluetooth/bluetooth.h>
#include <bluetooth/hci.h>
#include <bluetooth/hci_lib.h>
#include <unistd.h>
#include <bluetooth/rfcomm.h>
#include <iostream>
/**
 * @brief The Telegram class
 * Telegram class transport all information between Window class and
 * Network class.
 */
class Telegram {
private:
  byte* content;
  int   size;
public:
  //-----------------------------------------------------------------------
  Telegram() : content(NULL) , size(3) {
    content = new byte[size];
    content[0] = size-2;
    content[1] = 0x00;
    content[2] = 0x80; // Direct Command whitout response
  }

  //-----------------------------------------------------------------------
  Telegram(Telegram& source) {
    size = source.size;
    content = new byte[size];
    for (int i=0;i<size;i++) content[i] = source.content[i];
  }

  //-----------------------------------------------------------------------
  void append(byte piece) {
    byte* aux = new byte[size+1];
    for (int i=0;i<size;i++) aux[i] = content[i];
    aux[size] = piece;
    delete content;
    content = aux;
    size++;
    content[0] = size-2;
  }

  //-----------------------------------------------------------------------
  void append(byte* pieces, int count) {
    for (int i=0; i<count; i++) {
      append(pieces[i]);
    }
  }

  //-----------------------------------------------------------------------
  ~Telegram() {
    delete content;
  }

  //-----------------------------------------------------------------------
  bool send(int sock) {
    return write(sock, content, size);
  }
};



/**
 * @brief The Network class
 * Network class work as low level, allow send and recive information
 * of Bluetooth device connected.
 */
class Network {
private:
  QString macAddress;
  int     sock;
public:
  //-----------------------------------------------------------------------
  Network() {
  }

  //-----------------------------------------------------------------------
  QStringList scanDevices() {
    QStringList devices;

    inquiry_info *ii = NULL;
    int max_rsp, num_rsp;
    int dev_id, sock, len, flags;
    int i;
    char addr[19] = { 0 };
    char name[248] = { 0 };

    dev_id = hci_get_route(NULL);
    sock = hci_open_dev( dev_id );
    if (dev_id < 0 || sock < 0) {
        perror("opening socket");
        throw(1);
    }

    len  = 8;
    max_rsp = 255;
    flags = IREQ_CACHE_FLUSH;
    ii = (inquiry_info*)malloc(max_rsp * sizeof(inquiry_info));

    num_rsp = hci_inquiry(dev_id, len, max_rsp, NULL, &ii, flags);
    if( num_rsp <= 0 ) {
      throw(2);
    }

    for (i = 0; i < num_rsp; i++) {
        ba2str(&(ii+i)->bdaddr, addr);
        memset(name, 0, sizeof(name));
        if (hci_read_remote_name(sock, &(ii+i)->bdaddr, sizeof(name), name, 0) < 0) {
          strcpy(name, "unknown");
        }
        char text[30];
        sprintf(text,"%s  [%s]",addr,name);
        devices.append(text);
        //printf("%s  %s\n", addr, name);
    }

    free( ii );
    close( sock );
    return devices;
  }

  //-----------------------------------------------------------------------
  bool bind(QString address) {
    macAddress = address;
    struct sockaddr_rc addr; //  = { 0 };
    sock = socket(AF_BLUETOOTH, SOCK_STREAM, BTPROTO_RFCOMM);
    addr.rc_family = AF_BLUETOOTH;
    addr.rc_channel = (uint8_t) 1;
    str2ba( macAddress.toStdString().c_str(), &addr.rc_bdaddr );
    if ( connect(sock, (struct sockaddr *)&addr, sizeof(addr)) == 0) {
      return true;
    }
    else {
      return false;
    }
  }

  //-----------------------------------------------------------------------
  void unbind() {
    close(sock);
  }

  //-----------------------------------------------------------------------
  bool directCommand(Telegram t) {
    t.send(sock);
    return true;
  }

  //-----------------------------------------------------------------------
  bool directCommand(byte* pieces, int count) {
    Telegram t;
    t.append(pieces, count);
    t.send(sock);
    return true;
  }


};

#endif // NETWORK_H
