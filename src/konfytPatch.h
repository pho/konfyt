/******************************************************************************
 *
 * Copyright 2017 Gideon van der Kolf
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

#ifndef KONFYT_PATCH_H
#define KONFYT_PATCH_H

#include <QFile>
#include <QXmlStreamWriter>
#include <QXmlStreamReader>

#include "konfytMidiFilter.h"
#include "konfytPatchLayer.h"

#define DEFAULT_GAIN_FOR_NEW_LAYER 0.8


class konfytPatch
{
public:
    konfytPatch();

    // ----------------------------------------------------
    // Patch Info
    // ----------------------------------------------------
    QString getName();
    void setName(QString newName);
    QString getNote();
    void setNote(QString newNote);
    int id_in_project; // Unique ID in project to identify the patch in runtime.

    // ----------------------------------------------------
    // General layer related functions
    // ----------------------------------------------------
    QList<konfytPatchLayer> getLayerItems();
    konfytPatchLayer getLayerItem(konfytPatchLayer item);
    int getNumLayers();
    bool isValidLayerNumber(int layer);
    void removeLayer(konfytPatchLayer *layer);
    void clearLayers();
    void replaceLayer(konfytPatchLayer newLayer);
    void setLayerFilter(konfytPatchLayer* layer, konfytMidiFilter newFilter);
    float getLayerGain(konfytPatchLayer* layer);
    void setLayerGain(konfytPatchLayer* layer, float newGain);
    void setLayerSolo(konfytPatchLayer* layer, bool solo);
    void setLayerMute(konfytPatchLayer* layer, bool mute);
    void setLayerBus(konfytPatchLayer* layer, int bus);

    // ----------------------------------------------------
    // Soundfont layer related functions
    // ----------------------------------------------------

    konfytPatchLayer addProgram(konfytSoundfontProgram p);
    konfytPatchLayer addSfLayer(layerSoundfontStruct newSfLayer);
    layerSoundfontStruct getSfLayer(int id_in_engine);
    konfytPatchLayer getSfLayer_LayerItem(int id_in_engine);
    konfytSoundfontProgram getProgram(int id_in_engine);
    int getNumSfLayers();
    bool isValid_Sf_LayerNumber(int SfLayer);
    QList<konfytPatchLayer> getSfLayerList();
    float getSfLayerGain(int id_in_engine);
    void setSfLayerGain(int id_in_engine, float newGain);

    // ----------------------------------------------------
    // Carla Plugin functions
    // ----------------------------------------------------
    konfytPatchLayer addPlugin(layerCarlaPluginStruct newPlugin);
    layerCarlaPluginStruct getPlugin(int index_in_engine);
    konfytPatchLayer getPlugin_LayerItem(int index_in_engine);
    int getPluginCount();
    void setPluginGain(int index_in_engine, float newGain);
    float getPluginGain(int index_in_engine);
    QList<konfytPatchLayer> getPluginLayerList();

    // ----------------------------------------------------
    // Midi routing
    // ----------------------------------------------------

    QList<int> getMidiOutputPortList_ids();
    QList<layerMidiOutStruct> getMidiOutputPortList_struct();
    konfytPatchLayer addMidiOutputPort(int newPort);
    konfytPatchLayer addMidiOutputPort(layerMidiOutStruct newPort);

    // ----------------------------------------------------
    // Audio input ports
    // ----------------------------------------------------

    QList<int> getAudioInPortList_ids();
    QList<layerAudioInStruct> getAudioInPortList_struct();
    konfytPatchLayer addAudioInPort(int newPort);
    konfytPatchLayer addAudioInPort(layerAudioInStruct newPort);

    // ----------------------------------------------------
    // Save/load functions
    // ----------------------------------------------------

    bool savePatchToFile(QString filename);
    bool loadPatchFromFile(QString filename);
    void writeMidiFilterToXMLStream(QXmlStreamWriter* stream, konfytMidiFilter f);
    konfytMidiFilter readMidiFilterFromXMLStream(QXmlStreamReader *r);


    void userMessage(QString msg);
    void error_abort(QString msg);

private:
    QString patchName;
    QString patchNote;  // Custom note for user instructions or to describe the patch

    // List of layers (all types, order is important for user in the patch)
    QList<konfytPatchLayer> layerList;

    int id_counter; // Counter for unique ID given to layeritem to uniquely identify them.

};

#endif // KONFYT_PATCH_H
