#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QIcon>
#include <QMainWindow>
#include <QInputDialog>
#include <QDebug>
#include <QLineEdit>
#include <QSizePolicy>
#include <QLabel>
#include <QWidget>
#include <QMessageBox>


#include "Interface/MyDisplay.h"
#include "Interface/Parser.h"

#include "Music/MusicCollection.h"
#include "Parsers/iTunesXmlHandler.h"
#include "Parsers/iTunesXmlWriter.h"

#include "Dialogs/PreferencesDialog.h"
#include "Dialogs/iTunesPage.h"

#include "Widgets/Grid.h"
#include "Widgets/Keypad.h"
#include "Widgets/PlayBox.h"
#include "Widgets/Playlist.h"
#include "Widgets/GridDisplay.h"
#include "Widgets/ColourMapDisplay.h"
#include "Widgets/OptionsDialog.h"

class MainWindow : public QMainWindow
{
  Q_OBJECT

public:
  MainWindow(Grid* grid_);
  ~MainWindow();

  // sness - I should move this to a getter function, really.  and do
  // it with signals and slots.  Please fix me.
  GridDisplay *_display;

signals:
  void libraryUpdated();
  void openPredictGridFile(QString);
  void savePredictGridFile(QString);
  void playModeChanged();
  void cancelButtonSignal();
  void fullScreenModeOff();
  void resetGrid();
  void alphaDecayChanged();

private slots:
  void openiTunesLibrary();
  void openCollectionFile();
  void updateCurrentlyPlaying(MusicTrack *);
  void about();
  void display();
  void openSavedGrid();
  void saveCurrentGrid();
  void changedPlayMode();
  void cancelButton();
  void openDefaultiTunes();
  void fullScreenMode(bool mode);
  void keyPressEvent(QKeyEvent *);
  void keyReleaseEvent(QKeyEvent *);
  void resetButtonPressed();
  void optionsDialogTriggered();

private:
  void createActions();
  void createToolbars();
  void createWindow();
  void createMenus();

  //Widgets
  Grid *_dataGrid;
  PlayBox *_playBox;
  Playlist *_playlist;
  Tracklist *_tracklist;
  ColourMapDisplay *_colourMapDisplay;

  //Dialogs

  QMenu *_fileMenu;
  QMenu *_openMenu;
  QMenu *_debugMenu;
  QMenu *_viewMenu;
  QMenu *_helpMenu;

  QToolBar *_toolbar;

  QAction *_openiTunesAction;
  QAction *_coutAction;
  QAction *_openCollectionAction;
  QAction *_exitAction;
  QAction *_aboutAction;
  QAction *_extractAction;;
  QAction *_predictAction;
  QAction *_trainingAction;
  QAction *_openPreferencesAction;
  QAction *_saveGridAction;
  QAction *_loadGridAction;
  QAction *_playModeAction;
  QAction *_cancelAction;
  QAction *_initAction;
  QAction *_normHashAction;
  QAction *_fullScreenAction;
  QAction *_colourMapMode;
  QAction *_resetButtonAction;
  QAction *_optionsDialogAction;

  QLabel* gridHeightLabel;
  QLabel* gridWidthLabel;

  QLineEdit* gridWidth;
  QLineEdit* gridHeight;

  MusicCollection *_library;
  bool isFullScreenMouse;
  // MidiListener *_midi;
};

#endif /* MAINWINDOW_h */
