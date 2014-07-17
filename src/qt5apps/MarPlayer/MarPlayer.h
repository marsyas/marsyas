/*
** Copyright (C) 1998-2013 George Tzanetakis <gtzan@cs.uvic.ca>
**
** This program is free software; you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation; either version 2 of the License, or
** (at your option) any later version.
**
** This program is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy of the GNU General Public License
** along with this program; if not, write to the Free Software
** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#ifndef MARPLAYER_H
#define MARPLAYER_H

#include "Mapper.h"
#include "ui_MarPlayerForm.h"

#include <QMainWindow>

/**
  \class MarPlayer
  \brief Main window of MarPlayer application

  Main windows essentially connects the MarPlayer.ui
  form (created using Designer) to the Mapper class which
  is used to communicate with MarsyasQt::System.
*/

class MarPlayer : public QMainWindow
{
  Q_OBJECT

public:
  MarPlayer();
  ~MarPlayer();

public slots:
  void open();
  void open(const QString&);
  void about();

private slots:
  void positionSlider(int);
  void seekPos();
  void play(const QString&);

private:
  void init();
  void createMenus();
  void createActions();
  void createConnections();

  Ui::MarPlayerForm ui;	// the main ui form created
  // using Designer
  QMenu*   fileMenu;
  QMenu*   helpMenu;
  QAction* openAct;
  QAction* aboutAct;

  Mapper* mapper_; // maps gui events to marsyas events
};

#endif
