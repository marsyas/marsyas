set(APP
	MarGrid2
)

set(APP_SOURCES 
	MainWindow.cpp
	Colormaps/GreyScaleColormap.cpp
	Colormaps/SpectraColormap.cpp
	Colormaps/Colormap.cpp
	Dialogs/iTunesPage.cpp
	Dialogs/PreferencesDialog.cpp
	Interface/Parser.cpp
	Logger/Logger.cpp
	Midi/MidiListener.cpp
	Midi/RtMidi.cpp
	Music/MusicCollection.cpp
	Music/MusicLink.cpp
	Music/MusicTrack.cpp
	Parsers/iTunesXmlHandler.cpp
	Parsers/iTunesXmlWriter.cpp
	Widgets/Grid.cpp
	Widgets/OSCReceiver.cpp
	Widgets/Keypad.cpp
	Widgets/PlayBox.cpp
	Widgets/Playlist.cpp
	Widgets/PlaylistItem.cpp
	Widgets/Tracklist.cpp
	Widgets/GridDisplay.cpp
	Widgets/ColourMapDisplay.cpp
	Widgets/SquareRenderArea.cpp
	Widgets/OptionsDialog.cpp
	main.cpp
)

set(APP_HEADERS 
	#Dialogs/ColorPage.h
	#Dialogs/ClassifierPage.h
	#Dialogs/iTunesPage.h
	Dialogs/PreferencesDialog.h
	Interface/MyDisplay.h
	Midi/MidiListener.h
	#Parsers/iTunesXmlHandler.h
	#Parsers/iTunesXmlWriter.h
	Widgets/Grid.h
	Widgets/OSCReceiver.h
	Widgets/Keypad.h
	#Widgets/PlayBox.h
	Widgets/Playlist.h
	#Widgets/PlaylistItem.h
	#Widgets/Tracklist.h
	Widgets/GridDisplay.h
	Widgets/ColourMapDisplay.h
	Widgets/SquareRenderArea.h
	Widgets/OptionsDialog.h
	MainWindow.h
)

set(APP_RESOURCES
	MarGrid2.qrc
)


marsyas_qt4app(${APP} "${APP_SOURCES}" "${APP_HEADERS}"
  "${APP_UI}" "${APP_RESOURCES}")

