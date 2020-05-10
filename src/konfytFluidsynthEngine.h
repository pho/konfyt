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

#ifndef KONFYT_FLUIDSYNTH_ENGINE_H
#define KONFYT_FLUIDSYNTH_ENGINE_H

#include "konfytDefines.h"
#include "konfytMidi.h"
#include "konfytStructs.h"

#include <fluidsynth.h>

#include <QObject>
#include <QMutex>


struct KfFluidSynth
{
    friend class KonfytFluidsynthEngine;

    ~KfFluidSynth()
    {
        if (synth) { delete_fluid_synth(synth); }
        if (settings) { delete_fluid_settings(settings); }
    }

protected:
    fluid_synth_t* synth = nullptr;
    fluid_settings_t* settings = nullptr;
    KonfytSoundfontProgram program;
    int soundfontIDinSynth;
};


class KonfytFluidsynthEngine : public QObject
{
    Q_OBJECT
public:
    explicit KonfytFluidsynthEngine(QObject *parent = 0);
    ~KonfytFluidsynthEngine();

    void initFluidsynth(double sampleRate);

    QMutex mutex;
    void processJackMidi(KfFluidSynth *synth, const KonfytMidiEvent* ev);
    int fluidsynthWriteFloat(KfFluidSynth *synth, void* leftBuffer, void* rightBuffer, int len);

    KfFluidSynth* addSoundfontProgram(KonfytSoundfontProgram p);
    void removeSoundfontProgram(KfFluidSynth *synth);

    float getGain(KfFluidSynth *synth);
    void setGain(KfFluidSynth *synth, float newGain);

    void error_abort(QString msg);

private:
    QList<KfFluidSynth*> synths;
    double mSampleRate = 44100;
    
signals:
    void userMessage(QString msg);
    
};

#endif // KONFYT_FLUIDSYNTH_ENGINE_H
