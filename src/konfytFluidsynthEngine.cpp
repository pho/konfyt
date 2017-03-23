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

#include "konfytFluidsynthEngine.h"
#include <stdio.h>
#include <iostream>

#define MIDI_CHANNEL_0 0 // Used to easily see where channel 0 is forced.

konfytFluidsynthEngine::konfytFluidsynthEngine(QObject *parent) :
    QObject(parent)
{
    synthUniqueIDCounter = 0;
    samplerate = 44100;
}


// Generate fluidsynth MIDI events based on buffer from Jack midi input.
void konfytFluidsynthEngine::processJackMidi(int ID, const konfytMidiEvent *ev)
{
    // TODO NB: handle case where we are in panic mode, and all-note-off etc. messages are
    // received, but the mutex is already locked. We have to queue it somehow for until the
    // mutex is unlocked, othewise the events are thrown away and panic mode doesn't do what
    // it's supposed to.

    // If we don't get the mutex immediately, don't block and wait for it.
    if ( !mutex.tryLock() ) {
        return;
    }

    Q_ASSERT( synthDataMap.contains(ID) );

    if ( (ev->type != MIDI_EVENT_TYPE_PROGRAM) || (ev->type != MIDI_EVENT_TYPE_SYSTEM) ) {

        // All MIDI events are sent to Fluidsynth on channel 0

        if (ev->type == MIDI_EVENT_TYPE_NOTEON) {
            fluid_synth_noteon( synthDataMap.value(ID).synth, MIDI_CHANNEL_0, ev->data1, ev->data2 );
        } else if (ev->type == MIDI_EVENT_TYPE_NOTEOFF) {
            fluid_synth_noteoff( synthDataMap.value(ID).synth, MIDI_CHANNEL_0, ev->data1 );
        } else if (ev->type == MIDI_EVENT_TYPE_CC) {
            fluid_synth_cc( synthDataMap.value(ID).synth, MIDI_CHANNEL_0, ev->data1, ev->data2 );
            // If we have received an all notes off, sommer kill all the sound also. This is probably a panic.
            if (ev->data1 == MIDI_CC_ALL_NOTES_OFF) {
                fluid_synth_all_sounds_off( synthDataMap.value(ID).synth, MIDI_CHANNEL_0 );
            }
        } else if (ev->type == MIDI_EVENT_TYPE_PITCHBEND) {
            // Fluidsynth expects a positive pitchbend value, i.e. centered around 8192, not zero.
            fluid_synth_pitch_bend( synthDataMap.value(ID).synth, MIDI_CHANNEL_0, ev->pitchbendValue_signed()+8192 );
        }
    }

    mutex.unlock();
}

// Calls fluid_synth_write_float for synth with specified ID, fills specified buffers and returns
// the fluidsynth's return value.
int konfytFluidsynthEngine::fluidsynthWriteFloat(int ID, void *leftBuffer, void *rightBuffer, int len)
{
    // If we don't get the mutex immediately, don't block and wait for it.
    if ( !mutex.tryLock() ) {
        return 0;
    }

    Q_ASSERT( synthDataMap.contains(ID) );

    int ret =  fluid_synth_write_float( synthDataMap.value(ID).synth, len,
                                    leftBuffer, NULL, NULL,
                                    rightBuffer, NULL, NULL);

    mutex.unlock();
    return ret;
}

// Adds a new soundfont engine and returns the unique ID. Returns -1 on error.
int konfytFluidsynthEngine::addSoundfontProgram(konfytSoundfontProgram p)
{
    konfytFluidSynthData s;

    // Create settings object
    s.settings = new_fluid_settings();
    if (s.settings == NULL) {
        emit userMessage("Failed to create fluidsynth settings.");
        return -1;
    }

    // Set settings if necessary
    fluid_settings_setnum(s.settings, "synth.sample-rate", this->samplerate);


    // Create the synthesizer
    s.synth = new_fluid_synth(s.settings);
    if (s.synth == NULL) {
        emit userMessage("Failed to create fluidsynth synthesizer.");
        return -1;
    }

    // Load soundfont file
    int sfID = fluid_synth_sfload(s.synth, p.parent_soundfont.toLocal8Bit().data(), 0);
    if (sfID == -1) {
        emit userMessage("Failed to load soundfont " + p.parent_soundfont);
        return -1;
    }

    // Set the program
    fluid_synth_program_select(s.synth, 0, sfID, p.bank, p.program);

    s.program = p;
    s.soundfontIDinSynth = sfID;

    // Create new unique ID for the fluidsynth synth/data which outside classes use to refer to it.
    s.ID = synthUniqueIDCounter++;

    synthDataMap.insert( s.ID, s );

    return s.ID;
}

void konfytFluidsynthEngine::removeSoundfontProgram(int ID)
{
    Q_ASSERT( synthDataMap.contains(ID) );

    konfytFluidSynthData s = synthDataMap.value(ID);

    mutex.lock();
    synthDataMap.remove(ID);
    mutex.unlock();

    delete_fluid_synth(s.synth);
    delete_fluid_settings(s.settings);
}


void konfytFluidsynthEngine::InitFluidsynth(double SampleRate)
{
    userMessage("Fluidsynth version " + QString(fluid_version_str()));
    this->samplerate = SampleRate;
    userMessage("Fluidsynth sample rate: " + n2s(samplerate));
}

float konfytFluidsynthEngine::getGain(int ID)
{
    Q_ASSERT( synthDataMap.contains(ID) );

    return fluid_synth_get_gain( synthDataMap.value(ID).synth );
}

void konfytFluidsynthEngine::setGain(int ID, float newGain)
{
    Q_ASSERT( synthDataMap.contains(ID) );

    fluid_synth_set_gain( synthDataMap.value(ID).synth, newGain );
}




// Print error message to stdout, and abort app.
void konfytFluidsynthEngine::error_abort(QString msg)
{
    std::cout << "\n" << "Konfyt ERROR, ABORTING: sfengine:" << msg.toLocal8Bit().constData();
    abort();
}
