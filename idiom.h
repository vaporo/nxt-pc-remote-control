#ifndef IDIOM_H
#define IDIOM_H

#include <QString>

enum idiomtype{
  ENG=0,SPA=1
};

/** ========================================================================
 * @brief The Idiom class is used to enable to "NXT PC Remote Contro" to
 * change systematically between two idioms: English and Spanish
 */
class Idiom {
public:
  idiomtype it;
  QString windowTitle[2];
  QString scanButtonLabel[2];
  QString connectButtonLabel[2];
  QString disconnectButtonLabel[2];
  QString menuRecentConnections[2];
  QString menuClearConnections[2];
  QString menuSelectIdiom[2];
  QString menuEnglish[2];
  QString menuSpanish[2];
  QString menuAbout[2];
  QString messageSearching[2];
  QString messageBluetoothDisabled[2];
  QString messageNearDivices[2];
  QString messageDeviceAvailable[2];
  QString imageInfo[2];
  QString options[2];
  QString motorsInUse[2];
  QString configurations[2];
  QString active[2];
  QString add[2];
  QString remove[2];
  QString save[2];
  QString left[2];
  QString right[2];
  QString loadFile[2];
  QString saveFile[2];

public:

  /** ----------------------------------------------------------------------
   * @brief Idiom constructor launch the attributes values.  This class, has
   * all text needed to show in application interface.
   */
  Idiom() : it(ENG) {
    windowTitle[ENG] = "NXT PC Remote Control";
    windowTitle[SPA] = "Control Remoto de PC para NXT";

    scanButtonLabel[ENG] = "Scan";
    scanButtonLabel[SPA] = "Buscar";

    connectButtonLabel[ENG] = "Connect";
    connectButtonLabel[SPA] = "Conectar";

    disconnectButtonLabel[ENG] = "Disconnect";
    disconnectButtonLabel[SPA] = "Desconectar";

    menuRecentConnections[ENG] = "Recent connections";
    menuRecentConnections[SPA] = "Conexiones Recientes";

    menuClearConnections[ENG] = "Clean connections";
    menuClearConnections[SPA] = "Limpiar conexiones";

    menuSelectIdiom[ENG] = "Switch language";
    menuSelectIdiom[SPA] = "Cambiar idioma";

    menuEnglish[ENG] = "English";
    menuEnglish[SPA] = "Ingles";

    menuSpanish[ENG] = "Spanish";
    menuSpanish[SPA] = "Espanol";

    menuAbout[ENG] = "About (Ver.0-34)";
    menuAbout[SPA] = "Acerca de (Ver.0-34)";

    messageSearching[ENG] = "Searching to devices...";
    messageSearching[SPA] = "Buscando Dispositivos...";

    messageBluetoothDisabled[ENG] = "Bluetooth disabled";
    messageBluetoothDisabled[SPA] = "Bluetooth deshabilitado";

    messageNearDivices[ENG] = "There isn't near devices";
    messageNearDivices[SPA] = "No hay dispositivos cercanos";

    messageDeviceAvailable[ENG] = "Device isn't available";
    messageDeviceAvailable[SPA] = "El dispositivo ya no esta disponible";

    imageInfo[ENG] = ":/images/info-eng.png";
    imageInfo[SPA] = ":/images/info-spa.png";

    options[ENG] = "Options";
    options[SPA] = "Opciones";

    motorsInUse[ENG] = "Motors in use";
    motorsInUse[SPA] = "Motores en funcionamiento";

    configurations[ENG] = "Configurations";
    configurations[SPA] = "Configuraciones";

    active[ENG] = "Active";
    active[SPA] = "Activo";

    add[ENG] = "Add";
    add[SPA] = "Agregar";

    remove[ENG] = "Remove";
    remove[SPA] = "Remover";

    save[ENG] = "Save";
    save[SPA] = "Guardar";

    left[ENG] = "Left";
    left[SPA] = "Izquierda";

    right[ENG] = "Right";
    right[SPA] = "Derecha";

    loadFile[ENG] = "Load File";
    loadFile[SPA] = "Abrir Archivo";

    saveFile[ENG] = "Save File";
    saveFile[SPA] = "Guardar Archivo";

  }

  /** ----------------------------------------------------------------------
   * @brief getText method return the current set idiom
   * @return first three letters of idom
   */
  QString getText() {
    switch (it) {
    case ENG: return "eng";
    case SPA: return "spa";
    }
  }

  /** ----------------------------------------------------------------------
   * @brief setIdiomType method, put a new selected idiom
   * @param new idiom (of idiomtype)
   */
  void setIdiomType(idiomtype newit)  {
    it = newit;
  }

  /** ----------------------------------------------------------------------
   * @brief the next methods return all attributes values, namely,
   * this methods return de messages in current idiom.
   */

  idiomtype getIdiomType()              { return it; }

  QString getWindowTitle()              { return windowTitle[it]; }
  QString getScanButtonLabel()          { return scanButtonLabel[it]; }
  QString getConnectButtonLabel()       { return connectButtonLabel[it]; }
  QString getDisconnectButtonLabel()    { return disconnectButtonLabel[it]; }
  QString getMenuRecentConnections()    { return menuRecentConnections[it]; }
  QString getMenuClearConnections()     { return menuClearConnections[it]; }
  QString getMenuSelectIdiom()          { return menuSelectIdiom[it]; }
  QString getMenuEnglish()              { return menuEnglish[it]; }
  QString getMenuSpanish()              { return menuSpanish[it]; }
  QString getMenuAbout()                { return menuAbout[it]; }
  QString getMessageSearching()         { return messageSearching[it]; }
  QString getMessageBluetoothDisabled() { return messageBluetoothDisabled[it]; }
  QString getMessageNearDivices()       { return messageNearDivices[it]; }
  QString getMessageDeviceAvailable()   { return messageDeviceAvailable[it]; }
  QString getImageInfo()                { return imageInfo[it]; }
  QString getOptions()                  { return options[it]; }
  QString getMotorsInUse()              { return motorsInUse[it]; }
  QString getConfigurations()           { return configurations[it]; }
  QString getActive()                   { return active[it]; }
  QString getAdd()                      { return add[it]; }
  QString getRemove()                   { return remove[it]; }
  QString getSave()                     { return save[it]; }
  QString getLeft()                     { return left[it]; }
  QString getRight()                    { return right[it]; }
  QString getLoadFile()                 { return loadFile[it]; }
  QString getSaveFile()                 { return saveFile[it]; }

};

#endif // IDIOM_H
