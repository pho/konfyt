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

#ifndef SFDATABASE_H
#define SFDATABASE_H

#include <QObject>
#include <QList>
#include <QStringList>
#include <QMap>
#include <QThread>
#include <QDir>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>

#include <fluidsynth.h>

#include "konfytPatch.h"
#include "konfytDbTree.h"
#include "konfytDbTreeItem.h"

#define konfytDatabaseSource_returnSfont 1

#define XML_DATABASE "database"


// ============================================================================
// konfytDatabaseWorker
// ============================================================================

class konfytDatabaseWorker : public QObject
{
    Q_OBJECT
public:
    explicit konfytDatabaseWorker();

    fluid_synth_t* synth;

public slots:
    void scanDirs(QString sfontDir, QString sfzDir, QString patchesDir, QList<KonfytSoundfont*> sfontIgnoreList);
    void sfontFromFile(QString filename, int source);

private:
    void scanDirForFiles(QString dirname, QStringList suffixes, QStringList &list);
    KonfytSoundfont* _sfontFromFile(QString filename);

signals:
    void userMessage(QString msg);
    void scanDirsFihished(QList<KonfytSoundfont*> sfonts, QStringList sfzs, QStringList patches);
    void scanDirsStatus(QString msg);
    void sfontFromFileFinished(KonfytSoundfont* sfont, int source);

};

// ============================================================================
// konfytDatabase
// ============================================================================

class konfytDatabase : public QObject
{
    Q_OBJECT

public:
    konfytDatabase();
    ~konfytDatabase();

    QList<KonfytSoundfont*> getSfontList();
    int getNumSfonts();
    QList<KonfytPatch> getPatchList();
    int getNumPatches();
    QStringList getSfzList();
    int getNumSfz();
    konfytDbTree* sfzTree;
    konfytDbTree* sfzTree_results;
    konfytDbTree* sfontTree;
    konfytDbTree* sfontTree_results;
    void buildSfzTree();
    void buildSfzTree_results();
    void buildSfontTree();
    void buildSfontTree_results();
    void compactTree(konfytDbTreeItem* item);

    void scanDirs(QString sfontsDir, QString sfzDir, QString patchesDir);

    // General-use operations to return sfont objects
    void returnSfont(QString filename);
    void returnSfont(KonfytSoundfontProgram p);

    void clearDatabase();
    void clearDatabase_exceptSoundfonts();

    bool saveDatabaseToFile(QString filename);
    bool loadDatabaseFromFile(QString filename);

    // Search functionality
    void searchProgram(QString str);    // Search all programs in all soundfonts and patches.
    int getNumSfontsResults();
    int getNumSfontProgramResults();
    QList<KonfytSoundfont*>       getResults_sfonts();
    QList<KonfytSoundfontProgram>    getResults_sfontPrograms(KonfytSoundfont *sf);
    QList<KonfytSoundfontProgram>    getResults_allPrograms();
    int getNumPatchesResults();
    QList<KonfytPatch>      getResults_patches();
    int getNumSfzResults();
    QStringList         getResults_sfz();

    void addPatch(QString filename);

public slots:

    void scanDirsFinished(QList<KonfytSoundfont*> sfonts, QStringList sfzs, QStringList patches);
    void sfontFromFileFinished(KonfytSoundfont* sfont, int source);
    void userMessageFromWorker(QString msg);
    void scanDirsStatusFromWorker(QString msg);

signals:
    // Signals intended for outside world
    void userMessage(QString message);
    void scanDirs_status(QString msg);
    void scanDirs_finished();
    void returnSfont_finished(KonfytSoundfont* sf);
    // Signals intended for worker thread
    void start_scanDirs(QString sfontDir, QString sfzDir, QString patchesDir, QList<KonfytSoundfont*> sfontIgnoreList);
    void start_sfontFromFile(QString filename, int source);

private:
    QList<KonfytSoundfont*> sfontlist;
    QList<KonfytPatch> patchList;
    QList<QString> patchFilenameList;
    QStringList sfzList;

    fluid_settings_t* settings;
    fluid_synth_t* synth;
    int initFluidsynth();

    QMap<QString, KonfytSoundfont*> sfontResults;      // Map (soundfont filename:soundfont pointer) holding search results
    QList<KonfytPatch> patchResults;
    QStringList sfzResults;

    QThread workerThread;
    konfytDatabaseWorker worker;
    QString _sfontsDir;
    QString _sfzDir;
    QString _patchesDir;

    void addSfont(KonfytSoundfont *sf);
    void addSfz(QString filename);

};

#endif // SFDATABASE_H
