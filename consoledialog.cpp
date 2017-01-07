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

#include "consoledialog.h"
#include "ui_consoledialog.h"

#include "mainwindow.h"

ConsoleDialog::ConsoleDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ConsoleDialog)
{
    ui->setupUi(this);
}

ConsoleDialog::~ConsoleDialog()
{
    delete ui;
}

void ConsoleDialog::userMessage(QString message)
{
    ui->textBrowser->append(message);
}

void ConsoleDialog::on_pushButton_Clear_clicked()
{
    ui->textBrowser->clear();
}


void ConsoleDialog::on_checkBox_ShowMidiEvents_clicked()
{
    ((MainWindow*)(this->parent()))->setConsoleShowMidiMessages( ui->checkBox_ShowMidiEvents->isChecked() );
}

void ConsoleDialog::setShowMidiEvents(bool show)
{
    ui->checkBox_ShowMidiEvents->setChecked( show );
}
