#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtGui>
#include <QIcon>
#include <QMainWindow>
#include <QDebug>
#include "Interface/MyDisplay.h"
#include "Interface/Parser.h"

// #include "Midi/MidiListener.h"

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

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	MainWindow(Grid* grid_);
	~MainWindow();

signals:
	void libraryUpdated();
	void openPredictGridFile(QString);
	void savePredictGridFile(QString);
	void playModeChanged();

private slots:
	void openiTunesLibrary();
	void openMarXmlLibrary();
	void openMarCsvLibrary();
	void updateCurrentlyPlaying(MusicTrack *);
	void about();
	void display();
	void openPreferences();
	void saveiTunesLibrary();
	void openSavedGrid();
	void saveCurrentGrid();
	void changedPlayMode();

private:
	void createActions();
	void createToolbars();
	void createWindow();
	void createMenus();

	//Widgets
	MyDisplay *_display;
	Grid *_dataGrid;
	PlayBox *_playBox;
	Playlist *_playlist;
	Tracklist *_tracklist;

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
	QAction *_saveGridAction;
	QAction *_loadGridAction;
	QAction *_playModeAction;

	MusicCollection *_library;
	// MidiListener *_midi;
};

#endif /* MAINWINDOW_h */
