/******************************************************************************
 *
 * Copyright 2020 Gideon van der Kolf
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

#ifndef KONFYTJACKSTRUCTS_H
#define KONFYTJACKSTRUCTS_H

#include "konfytMidiFilter.h"
#include "ringbufferqmutex.h"
#include "konfytFluidsynthEngine.h"

#include <jack/jack.h>


enum KonfytJackPortType {
    KonfytJackPortType_AudioIn  = 0,
    KonfytJackPortType_AudioOut = 1,
    KonfytJackPortType_MidiIn   = 2,
    KonfytJackPortType_MidiOut  = 3,
};

struct KonfytJackPortsSpec
{
    QString name;
    QString midiOutConnectTo;
    KonfytMidiFilter midiFilter;
    QString audioInLeftConnectTo;
    QString audioInRightConnectTo;
};

struct KfJackAudioPort
{
    friend class KonfytJackEngine;
protected:
    float gain = 1;
    jack_port_t* jackPointer = nullptr;
    void* buffer;
    QStringList connectionList;
    RingbufferQMutex<float> traffic{8192};
};

struct KfJackMidiPort
{
    friend class KonfytJackEngine;
protected:
    jack_port_t* jackPointer = nullptr;
    void* buffer;
    KonfytMidiFilter filter;
    QStringList connectionList;
    int noteOns = 0;
    bool sustainNonZero = false;
    bool pitchbendNonZero = false;
    RingbufferQMutex<KonfytMidiEvent> traffic{8192};
};

struct KfJackMidiRoute
{
    friend class KonfytJackEngine;
protected:
    bool active = false;
    bool prevActive = false;
    KonfytMidiFilter filter;
    KfJackMidiPort* source = nullptr;
    KfJackMidiPort* destPort = nullptr;
    KfFluidSynth* destFluidsynthID = nullptr;
    bool destIsJackPort = true;
    RingbufferQMutex<KonfytMidiEvent> eventsTxBuffer{100};
};

struct KfJackAudioRoute
{
    friend class KonfytJackEngine;
protected:
    bool active = false;
    bool prevActive = false;
    float gain = 1;
    unsigned int fadeoutCounter = 0;
    bool fadingOut = false;
    KfJackAudioPort* source = nullptr;
    KfJackAudioPort* dest = nullptr;
};

struct KfJackPluginPorts
{
    friend class KonfytJackEngine;
protected:
    KfFluidSynth* fluidSynthInEngine; // Id in plugin's respective engine (used for Fluidsynth)
    KfJackMidiPort* midi;        // Send midi output to plugin
    KfJackAudioPort* audioInLeft;  // Receive plugin audio
    KfJackAudioPort* audioInRight;
    KfJackMidiRoute* midiRoute = nullptr;
    KfJackAudioRoute* audioLeftRoute = nullptr;
    KfJackAudioRoute* audioRightRoute = nullptr;
};

struct KonfytJackNoteOnRecord
{
    int note;
    bool jackPortNotFluidsynth; // true for jack port, false for Fluidsynth
    KfFluidSynth* fluidSynth;
    KfJackMidiPort* port;
    KfJackMidiPort* sourcePort;
    KonfytMidiFilter filter;
    int globalTranspose;
};

struct KonfytJackConPair
{
    QString srcPort;
    QString destPort;

    QString toString()
    {
        return srcPort + " \u2B95 " + destPort;
    }

    bool equals(const KonfytJackConPair &a) {
        return ( (this->srcPort == a.srcPort) && (this->destPort == a.destPort) );
    }
};

struct KfJackMidiRxEvent
{
    KfJackMidiPort* sourcePort;
    KonfytMidiEvent midiEvent;
};


#endif // KONFYTJACKSTRUCTS_H
