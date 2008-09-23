set(APP
	MarGrid2
)
set( QT_USE_QTXML TRUE )
set(APP_SOURCES 
	MainWindow.cpp
	Colormaps/GreyScaleColormap
	Colormaps/SpectraColormap
	Colormaps/ColorMap
	Dialogs/iTunesPage.cpp
	Dialogs/PreferencesDialog.cpp
	Interface/Parser.cpp
	Logger/Logger.cpp
	Midi/MidiListener.cpp
	Midi/RtMidi.cpp
	Music/MusicCollection.cpp
	Music/MusicLink.cpp
	Music/MusicTrack.cpp
	Parser/iTunesXmlHandler.cpp
	Parser/iTunesXmlWriter.cpp
	Widgets/Grid.cpp
	Widgets/Keypad.cpp
	Widgets/Playbox.cpp
	Widgets/Playlist.cpp
	Widgets/PlaylistItem.cpp
	Widgets/PlaylistModel.cpp
	Widgets/Tracklist.cpp
	main.cpp
)

set(APP_HEADERS 
	Dialogs/ColorPage.h
	Dialogs/ClassifierPage.h
	Dialogs/iTunesPage.h
	Dialogs/PreferencesDialog.h
	Colormaps/GreyScaleColormap.h
	Colormaps/SpectraColormap.h
	Colormaps/ColorMap.h
	Interface/Classifier.h
	Interface/Extractor.h
	Interface/Playback.h
	Interface/MyDisplay.h
	Interface/Parser.h
	Logger/Logger.h
	Midi/RtError.h
	Midi/RtMidi.h
	Midi/MidiListener.h
	Music/MusicCollection.h
	Music/MusicLink.h
	Music/MusicTrack.h
	Parser/iTunesXmlHandler.h
	Parser/iTunesXmlWriter.h
	Widgets/Grid.h
	Widgets/Keypad.h
	Widgets/Playbox.h
	Widgets/Playlist.h
	Widgets/PlaylistItem.h
	Widgets/PlaylistModel.h
	Widgets/Tracklist.h
	MainWindow.h
)

set(APP_RESOURCES
	MarGrid2.qrc
)


marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

