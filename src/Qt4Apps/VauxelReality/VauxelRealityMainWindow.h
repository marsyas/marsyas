
#ifndef VAUXELREALITYMAINWINDOW_H_
#define VAUXELREALITYMAINWINDOW_H_

#include <QtGui>

#include "AbstractMarClusteringViewer.h"
#include "AbstractMarClusteringModel.h"
#include "AbstractMarExtractor.h"

#include "PlayGroupBoxW.h"
#include "PlayListW.h"
#include "TrackListW.h"

#include "PreferencesDialog.h"

#include "MusicCollection.h"

class VauxelRealityMainWindow : public QMainWindow
{
  Q_OBJECT

public:
  VauxelRealityMainWindow();
  ~VauxelRealityMainWindow();

signals:
  void libraryUpdated();

private slots:
  void openiTunesLibrary();
  void openMarXmlLibrary();
  void openMarCsvLibrary();
  void updateCurrentlyPlaying(MusicTrack *);
  void about();
  void display();
  void openPreferences();
  void saveiTunesLibrary();

private:
  void createActions();
  void createToolbars();
  void createWindow();
  void createMenus();

  AbstractMarExtractor *_extractor;
  AbstractMarClusteringModel *_model;

  //Widgets
  AbstractMarClusteringViewer *_display;
  PlayGroupBoxW *_playBox;
  PlayListW *_playlist;
  TrackListW *_tracklist;

  //Dialogs
  PreferencesDialog *_preferencesDialog;

  QMenu *_fileMenu;
  QMenu *_openMenu;
  QMenu *_debugMenu;
  QMenu *_helpMenu;

  QToolBar *_toolbar;

  QAction *_openiTunesAction;
  QAction *_openXmlAction;
  QAction *_openCsvAction;
  QAction *_coutAction;
  QAction *_exitAction;
  QAction *_aboutAction;
  QAction *_extractAction;;
  QAction *_predictAction;
  QAction *_trainingAction;
  QAction *_openPreferencesAction;
  QAction *_saveAction;

  MusicCollection *_library;

};

#endif // VAUXELREALITYMAINWINDOW_H_