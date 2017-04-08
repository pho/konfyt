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

#ifndef KONFYT_STRUCTS_H
#define KONFYT_STRUCTS_H

#include <QString>
#include <jack/jack.h>


/* This represents a program/preset within a soundfont (i.e. a single voice/instrument). */
typedef struct konfytSoundfontProgram_t {

    QString name;               // Program name
    int bank;                   // Bank number
    int program;                // Program/preset number
    QString parent_soundfont;   // Filename of parent soundfont

    // Constructor
    konfytSoundfontProgram_t() : bank(0), program(0) {}

} konfytSoundfontProgram;

typedef struct konfytSoundfont_t {

    QString filename;
    QString name;
    QList<konfytSoundfontProgram> programlist;
    QList<konfytSoundfontProgram> searchResults;

} konfytSoundfont;





#endif // KONFYT_STRUCTS_H
