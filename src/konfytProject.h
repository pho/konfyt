/******************************************************************************
 *
 * Copyright 2023 Gideon van der Kolf
 *
 * This file is part of Konfyt.
 *
 *     Konfyt is free software: you can redistribute it and/or modify
 *     it under the terms of the GNU General Public License as published by
 *     the Free Software Foundation, either version 3 of the License, or
 *     (at your option) any later version.
 *
 *     Konfyt is distributed in the hope that it will be useful,
 *     but WITHOUT ANY WARRANTY; without even the implied warranty of
 *     MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *     GNU General Public License for more details.
 *
 *     You should have received a copy of the GNU General Public License
 *     along with Konfyt.  If not, see <http://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

#ifndef KONFYT_PROJECT_H
#define KONFYT_PROJECT_H

#include "konfytDefines.h"
#include "konfytJackStructs.h"
#include "konfytPatch.h"
#include "konfytStructs.h"

#include <QDir>
#include <QFile>
#include <QObject>
#include <QSharedPointer>
#include <QStringList>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>


#define PROJECT_FILENAME_EXTENSION ".konfytproject"
#define PROJECT_PATCH_DIR "patches"


class KonfytProject;
typedef QSharedPointer<KonfytProject> ProjectPtr;


struct PrjAudioBus
{
    QString busName;
    KfJackAudioPort* leftJackPort = nullptr;
    float leftGain = 1;
    QStringList leftOutClients;
    KfJackAudioPort* rightJackPort = nullptr;
    float rightGain = 1;
    QStringList rightOutClients;
    bool ignoreMasterGain = false;
};

struct PrjAudioInPort
{
    QString portName;
    KfJackAudioPort* leftJackPort = nullptr;
    KfJackAudioPort* rightJackPort = nullptr;
    float leftGain = 1;
    float rightGain = 1;
    QStringList leftInClients;
    QStringList rightInClients;
};

struct PrjMidiPort
{
    QString portName;
    QStringList clients;
    KfJackMidiPort* jackPort = nullptr; // TODO THIS MUST NOT BE IN PROJECT
    KonfytMidiFilter filter = KonfytMidiFilter::allPassFilter();
};

struct KonfytTrigger
{
    QString actionText;
    int type = -1;
    int channel = 0;
    int data1 = -1;
    int bankMSB = -1;
    int bankLSB = -1;

    int toInt() const
    {
        return hashMidiEventToInt(type, channel, data1, bankMSB, bankLSB);
    }
    QString toString()
    {
        return midiEventToString(type, channel, data1, bankMSB, bankLSB);
    }
};

struct ExternalApp
{
    ExternalApp() {}
    ExternalApp(QString name, QString cmd) : friendlyName(name), command(cmd) {}

    QString friendlyName;
    QString command;
    bool runAtStartup = false;
    bool autoRestart = false;
};

enum portLeftRight { leftPort, rightPort };

class KonfytProject : public QObject
{
    Q_OBJECT
public:
    explicit KonfytProject(QObject *parent = 0);

    bool loadProject(QString filename);
    bool saveProject();
    bool saveProjectAs(QString dirname);

    void addPatch(KonfytPatch* newPatch);
    void insertPatch(KonfytPatch* newPatch, int index);
    KonfytPatch* removePatch(int i);
    void movePatch(int indexFrom, int indexTo);
    KonfytPatch* getPatch(int i);
    int getPatchIndex(KonfytPatch* patch);
    QList<KonfytPatch*> getPatchList();
    int getNumPatches();

    QString getDirname();
    void setDirname(QString newDirname);
    QString getProjectName();
    void setProjectName(QString newName);
    bool getShowPatchListNumbers();
    void setShowPatchListNumbers(bool show);
    bool getShowPatchListNotes();
    void setShowPatchListNotes(bool show);
    void setMidiPickupRange(int range);
    int getMidiPickupRange();

    // MIDI input ports
    QList<int> midiInPort_getAllPortIds();  // Get list of port ids
    int midiInPort_addPort(QString portName);
    void midiInPort_removePort(int portId);
    bool midiInPort_exists(int portId);
    PrjMidiPort midiInPort_getPort(int portId);
    int midiInPort_getPortIdWithJackId(KfJackMidiPort *jackPort);
    int midiInPort_getFirstPortId(int skipId);
    int midiInPort_count();
    void midiInPort_setName(int portId, QString name);
    void midiInPort_setJackPort(int portId, KfJackMidiPort* jackport);
    QStringList midiInPort_getClients(int portId);  // Get client list of single port
    void midiInPort_addClient(int portId, QString client);
    void midiInPort_removeClient(int portId, QString client);
    void midiInPort_setPortFilter(int portId, KonfytMidiFilter filter);

    // MIDI output ports
    QList<int> midiOutPort_getAllPortIds();  // Get list of port ids
    int midiOutPort_addPort(QString portName);
    void midiOutPort_removePort(int portId);
    bool midiOutPort_exists(int portId) const;
    PrjMidiPort midiOutPort_getPort(int portId) const;
    int midiOutPort_count();
    void midiOutPort_setName(int portId, QString name);
    void midiOutPort_setJackPort(int portId, KfJackMidiPort* jackport);
    QStringList midiOutPort_getClients(int portId);  // Get client list of single port
    void midiOutPort_addClient(int portId, QString client);
    void midiOutPort_removeClient(int portId, QString client);

    // Audio input ports
    QList<int> audioInPort_getAllPortIds(); // Get list of port ids
    int audioInPort_add(QString portName);
    void audioInPort_remove(int portId);
    bool audioInPort_exists(int portId) const;
    PrjAudioInPort audioInPort_getPort(int portId) const;
    int audioInPort_count();
    void audioInPort_setName(int portId, QString name);
    void audioInPort_setJackPorts(int portId, KfJackAudioPort* left, KfJackAudioPort* right);
    void audioInPort_addClient(int portId, portLeftRight leftRight, QString client);
    void audioInPort_removeClient(int portId, portLeftRight leftRight, QString client);

    // Audio buses
    int audioBus_add(QString busName);
    void audioBus_remove(int busId);
    int audioBus_count();
    bool audioBus_exists(int busId);
    PrjAudioBus audioBus_getBus(int busId);
    int audioBus_getFirstBusId(int skipId);
    QList<int> audioBus_getAllBusIds();
    void audioBus_replace(int busId, PrjAudioBus newBus);
    void audioBus_replace_noModify(int busId, PrjAudioBus newBus);
    void audioBus_addClient(int busId, portLeftRight leftRight, QString client);
    void audioBus_removeClient(int busId, portLeftRight leftRight, QString client);

    // External Apps
    int addExternalApp(ExternalApp app);
    void removeExternalApp(int id);
    ExternalApp getExternalApp(int id);
    QList<int> getExternalAppIds();
    bool hasExternalAppWithId(int id);
    void modifyExternalApp(int id, ExternalApp app);

    // Triggers
    void addAndReplaceTrigger(KonfytTrigger newTrigger);
    void removeTrigger(QString actionText);
    QList<KonfytTrigger> getTriggerList();
    bool isProgramChangeSwitchPatches();
    void setProgramChangeSwitchPatches(bool value);

    // Other JACK MIDI connections
    KonfytJackConPair addJackMidiCon(QString srcPort, QString destPort);
    QList<KonfytJackConPair> getJackMidiConList();
    KonfytJackConPair removeJackMidiCon(int i);
    // Other JACK Audio connections
    KonfytJackConPair addJackAudioCon(QString srcPort, QString destPort);
    QList<KonfytJackConPair> getJackAudioConList();
    KonfytJackConPair removeJackAudioCon(int i);

    bool isModified(); // Returns whether the project has been modified since last load/save.
    void setModified(bool mod);

signals:
    void print(QString msg);
    void projectModifiedChanged(bool modified); // Emitted when project modified state changes.
    void midiInPortNameChanged(int portId);
    void midiOutPortNameChanged(int portId);
    void audioInPortNameChanged(int portId);
    void midiPickupRangeChanged(int range);

    void externalAppAdded(int id);
    void externalAppRemoved(int id);
    void externalAppModified(int id);
    
private:
    QList<KonfytPatch*> patchList;
    QString projectDirname;
    QString projectName = "New Project";

    QMap<int, PrjMidiPort> midiInPortMap;
    int midiInPort_getUniqueId();

    QMap<int, PrjMidiPort> midiOutPortMap;
    int midiOutPort_getUniqueId();

    QMap<int, PrjAudioInPort> audioInPortMap;
    int audioInPort_getUniqueId();

    QMap<int, PrjAudioBus> audioBusMap;
    int audioBus_getUniqueId();

    int getUniqueIdHelper(QList<int> ids);

    QMap<int, ExternalApp> externalApps;
    int getUniqueExternalAppId();
    void clearExternalApps();
    void writeExternalApps(QXmlStreamWriter* w);

    void preExternalAppsRead();
    QList<ExternalApp> tempExternalAppList;
    void readExternalApps(QXmlStreamReader* r);
    void postExternalAppsRead();

    QHash<QString, KonfytTrigger> triggerHash;
    bool programChangeSwitchPatches = true;
    bool patchListNumbers = true;
    bool patchListNotes = false;
    int midiPickupRange = 127;

    QList<KonfytJackConPair> jackMidiConList;
    QList<KonfytJackConPair> jackAudioConList;

    bool modified = false; // Whether project has been modified since last load/save.

private:
    const char* XML_PRJ = "konfytProject";
    const char* XML_PRJ_NAME = "name";
    const char* XML_PRJ_PATCH = "patch";
    const char* XML_PRJ_PATCH_FILENAME = "filename";
    const char* XML_PRJ_PATCH_LIST_NUMBERS = "patchListNumbers";
    const char* XML_PRJ_PATCH_LIST_NOTES = "patchListNotes";
    const char* XML_PRJ_MIDI_PICKUP_RANGE = "midiPickupRange";
    const char* XML_PRJ_MIDI_IN_PORTLIST = "midiInPortList";
    const char* XML_PRJ_MIDI_IN_PORT = "port";
    const char* XML_PRJ_MIDI_IN_PORT_ID = "portId";
    const char* XML_PRJ_MIDI_IN_PORT_NAME = "portName";
    const char* XML_PRJ_MIDI_IN_PORT_CLIENT = "client";
    const char* XML_PRJ_MIDI_OUT_PORTLIST = "midiOutPortList";
    const char* XML_PRJ_MIDI_OUT_PORT = "port";
    const char* XML_PRJ_MIDI_OUT_PORT_ID = "portId";
    const char* XML_PRJ_MIDI_OUT_PORT_NAME = "portName";
    const char* XML_PRJ_MIDI_OUT_PORT_CLIENT = "client";
    const char* XML_PRJ_BUSLIST = "audioBusList";
    const char* XML_PRJ_BUS = "bus";
    const char* XML_PRJ_BUS_ID = "busId";
    const char* XML_PRJ_BUS_NAME = "busName";
    const char* XML_PRJ_BUS_LGAIN = "leftGain";
    const char* XML_PRJ_BUS_RGAIN = "rightGain";
    const char* XML_PRJ_BUS_IGNORE_GLOBAL_VOLUME = "ignoreGlobalVolume";
    const char* XML_PRJ_BUS_LCLIENT = "leftClient";
    const char* XML_PRJ_BUS_RCLIENT = "rightClient";
    const char* XML_PRJ_AUDIOINLIST = "audioInputPortList";
    const char* XML_PRJ_AUDIOIN_PORT = "port";
    const char* XML_PRJ_AUDIOIN_PORT_ID = "portId";
    const char* XML_PRJ_AUDIOIN_PORT_NAME = "portName";
    const char* XML_PRJ_AUDIOIN_PORT_LGAIN = "leftGain";
    const char* XML_PRJ_AUDIOIN_PORT_RGAIN = "rightGain";
    const char* XML_PRJ_AUDIOIN_PORT_LCLIENT = "leftClient";
    const char* XML_PRJ_AUDIOIN_PORT_RCLIENT = "rightClient";

    // TODO DEPRECATED - replaced by EXT_APP below
    const char* XML_PRJ_PROCESSLIST = "processList";
    const char* XML_PRJ_PROCESS = "process";
    const char* XML_PRJ_PROCESS_APPNAME = "appname";

    const char* XML_PRJ_EXT_APP_LIST = "externalAppList";
    const char* XML_PRJ_EXT_APP = "externalApp";
    const char* XML_PRJ_EXT_APP_NAME = "friendlyName";
    const char* XML_PRJ_EXT_APP_CMD = "command";
    const char* XML_PRJ_EXT_APP_RUNATSTARTUP = "runAtStartup";
    const char* XML_PRJ_EXT_APP_RESTART = "autoRestart";

    const char* XML_PRJ_TRIGGERLIST = "triggerList";
    const char* XML_PRJ_TRIGGER = "trigger";
    const char* XML_PRJ_TRIGGER_ACTIONTEXT = "actionText";
    const char* XML_PRJ_TRIGGER_TYPE = "type";
    const char* XML_PRJ_TRIGGER_CHAN = "channel";
    const char* XML_PRJ_TRIGGER_DATA1 = "data1";
    const char* XML_PRJ_TRIGGER_BANKMSB = "bankMSB";
    const char* XML_PRJ_TRIGGER_BANKLSB = "bankLSB";
    const char* XML_PRJ_PROG_CHANGE_SWITCH_PATCHES = "programChangeSwitchPatches";
    const char* XML_PRJ_OTHERJACK_MIDI_CON_LIST = "otherJackMidiConList";
    const char* XML_PRJ_OTHERJACK_AUDIO_CON_LIST = "otherJackAudioConList";
    const char* XML_PRJ_OTHERJACKCON = "otherJackCon";
    const char* XML_PRJ_OTHERJACKCON_SRC = "srcPort";
    const char* XML_PRJ_OTHERJACKCON_DEST = "destPort";
};

#endif // KONFYT_PROJECT_H
